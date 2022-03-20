// winapi
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// aik_process
#include <aik/internal/driver.hpp>
#include <aik/internal/structs.hpp>
#include <aik/internal/process_module.hpp>
#include <aik/internal/page.hpp>

// utils
#include <aik/utils/utils.hpp>

// C++ STL
#include <vector>

namespace {
constexpr auto kFileMappingName = "__aik";
constexpr auto kDeviceName      = R"(\\.\PEAuth)";
constexpr auto kRegSubKey       = "software\\aik";
constexpr auto kPid             = "__p";
}// namespace

// static image base offset pointer to operation manager struct
namespace {
    volatile std::uint64_t static_memory_pointer = 0xF1023FAB401234FA;
}

aik::internal::driver::driver() {
    // global singleton
    auto handle = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            1,
            kFileMappingName
    );
    if (handle == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        throw std::runtime_error("Error creating file mapping");
    }

    m_device = CreateFileA(
            kDeviceName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_NEW,
            0,
            NULL
    );

    if (m_device == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Error creating device");
    }

    PVOID mem = VirtualAlloc(0, sizeof(internal::request_data), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (mem == NULL) {
        throw std::runtime_error("Memory allocation error");
    }

    static_memory_pointer = reinterpret_cast<std::uint64_t>(mem);

    auto pid = reinterpret_cast<PVOID>(GetCurrentProcessId());
    utils::write_registry(HKEY_LOCAL_MACHINE, kRegSubKey, kPid, &pid, REG_QWORD, 8);

    if (!NT_SUCCESS(dispatch_request(static_cast<aik::internal::request_type>(99), nullptr))) {
        throw std::runtime_error("Initialization signal returned error");
    }
}

NTSTATUS aik::internal::driver::read_memory(const std::uint64_t src_addr, const std::uint32_t pid, void* const tgt_addr, const std::uint64_t size) const {
    if (m_device == INVALID_HANDLE_VALUE || m_device == 0) {
        throw std::runtime_error("Driver uninitialised");
    }

    NTSTATUS status;
    aik::internal::request_read request{};

    request.pid           = pid;
    request.dest          = tgt_addr;
    request.src           = reinterpret_cast<PVOID>(src_addr);
    request.size          = size;
    request.physical_mode = false;

    status = dispatch_request(aik::internal::kRead, &request);
    if (!NT_SUCCESS(status)) {
        std::printf("Read process memory status: 0x%lX\n", status);
    }
    return status;
}

NTSTATUS aik::internal::driver::write_memory(const std::uint64_t tgt_addr, const std::uint32_t pid, const void* const src_addr, const std::uint64_t size) const {
    if (m_device == INVALID_HANDLE_VALUE || m_device == 0) {
        throw std::runtime_error("Driver uninitialised");
    }

    NTSTATUS status;
    aik::internal::request_write request{};

    request.pid           = pid;
    request.src           = const_cast<PVOID>(src_addr);
    request.dest          = reinterpret_cast<PVOID>(tgt_addr);
    request.size          = size;
    request.physical_mode = false;

    status = dispatch_request(aik::internal::kWrite, &request);
    if (!NT_SUCCESS(status)) {
        std::printf("Write process memory status: 0x%lX\n", status);
    }
    return status;
}

NTSTATUS aik::internal::driver::find_module(aik::internal::process_module& _process_module, const std::uint32_t pid, const std::wstring& module_name, bool list_pages) const {
    if (m_device == INVALID_HANDLE_VALUE || m_device == 0) {
        throw std::runtime_error("Driver uninitialised");
    }

    NTSTATUS status;
    uint64_t base_addr = 0;
    // TODO: change to 64 bits in whole project
    uint32_t size = 0;
    std::vector<page> pages;
    pages.resize(constants::kModulePagesAmount);

    aik::internal::request_module request{};

    request.pid        = pid;
    request.res_addr   = &base_addr;
    request.res_size   = reinterpret_cast<DWORD*>(&size);
    request.list_pages = list_pages;
    request.pages      = &pages[0];
    wcscpy_s(request.module, module_name.length() + 1, module_name.c_str());

    status = dispatch_request(aik::internal::kModule, &request);
    if (!NT_SUCCESS(status)) {
        std::printf("Find process module status: 0x%lX\n", status);
        return status;
    }

    for (std::size_t i = 0; i < constants::kModulePagesAmount; i++) {
        if (!pages[i].get_address() || !pages[i].get_size()) {
            pages.resize(i);
            pages.shrink_to_fit();
            break;
        }
    }

    _process_module = process_module(base_addr, size, pages);
    return status;
}

NTSTATUS aik::internal::driver::get_base_addr(std::uint64_t &base_addr, const std::uint32_t pid) const {
    if (m_device == INVALID_HANDLE_VALUE || m_device == 0) {
        throw std::runtime_error("Driver uninitialised");
    }

    NTSTATUS status;
    aik::internal::request_mainbase request{};

    request.pid      = pid;
    request.res_addr = &base_addr;

    status = dispatch_request(aik::internal::request_type::kMainbase, &request);
    if (!NT_SUCCESS(status)) {
        std::printf("Get process base address status: 0x%lX\n", status);
    }
    return status;
}

NTSTATUS aik::internal::driver::list_pages_range(std::vector<page>& pages, const std::uint32_t pid, const std::uint64_t begin_addr, const std::uint64_t size) const {
    if (m_device == INVALID_HANDLE_VALUE || m_device == 0) {
        throw std::runtime_error("Driver uninitialised");
    }
    pages.resize(constants::kPagesArrSz);

    NTSTATUS status;
    aik::internal::request_pages  request{};

    request.pid        = pid;
    request.begin_addr = reinterpret_cast<PVOID>(begin_addr);
    request.size       = size;
    request.pages      = &pages[0];

    status = dispatch_request(aik::internal::request_type::kPages, &request);
    if (!NT_SUCCESS(status)) {
        std::printf("List process pages status: 0x%lX\n", status);
    }

    for (std::size_t i = 0; i < constants::kPagesArrSz; i++) {
        if (!pages[i].get_address() || !pages[i].get_size()) {
            pages.resize(i);
            pages.shrink_to_fit();
            break;
        }
    }

    return status;
}

NTSTATUS aik::internal::driver::find_pattern(std::uint64_t& pattern_addr,
                                             const std::uint32_t pid,
                                             const std::uint64_t begin_addr,
                                             const std::uint64_t size,
                                             const std::string_view& pattern,
                                             const std::bitset<256>& mask) const {
    if (m_device == INVALID_HANDLE_VALUE || m_device == 0) {
        throw std::runtime_error("Driver uninitialised");
    }

    NTSTATUS status;
    std::vector<page> pages;

    status = list_pages_range(pages, pid, begin_addr, size);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    for (const auto& page : pages) {
        char* buffer = new char[page.get_size()];
        if (!NT_SUCCESS(read_memory(page.get_address(), pid, buffer, page.get_size()))) {
            delete[] buffer;
            continue;
        }

        pattern_addr = utils::find_pattern(buffer, page.get_address(), page.get_size(), pattern, mask);
        if (!pattern_addr) {
            delete[] buffer;
            continue;
        }

        delete[] buffer;
        return STATUS_SUCCESS;
    }

    return STATUS_NOT_FOUND;
}

NTSTATUS aik::internal::driver::dispatch_request(const aik::internal::request_type req_type, PVOID args) const {
    NTSTATUS status;
    aik::internal::request_data data{req_type, args, &status};
    if (memcpy_s(reinterpret_cast<void*>(static_memory_pointer), sizeof(internal::request_data), &data, sizeof(data))) {
        throw std::runtime_error("memcpy_s error");
    }

    FlushFileBuffers(m_device);
    return status;
}
