#pragma once

// C++ STL
#include <bitset>
#include <string>
#include <vector>

// forward declarations
namespace aik::internal {
using UINT = unsigned int;
enum request_type : UINT;
class process_module;
class page;
}// namespace aik_process::internal

namespace aik::internal {
class driver {
public:
    using NTSTATUS = long;
    using HANDLE   = void*;
    using PVOID    = void*;

    ~driver()                            = default;

    driver(const driver&)                = delete;
    driver& operator=(const driver&)     = delete;

    driver(driver&&)            noexcept = default;
    driver& operator=(driver&&) noexcept = default;

    driver();

    NTSTATUS read_memory(std::uint64_t src_addr, std::uint32_t pid, void* tgt_addr, std::uint64_t size) const;
    NTSTATUS write_memory(std::uint64_t tgt_addr, std::uint32_t pid, const void* src_addr, std::uint64_t size) const;

    NTSTATUS find_module(process_module& process_module, std::uint32_t pid, const std::wstring& module_name, bool list_pages = false) const;
    NTSTATUS get_base_addr(std::uint64_t& base_addr, std::uint32_t pid) const;
    NTSTATUS list_pages_range(std::vector<page>& pages, std::uint32_t pid, std::uint64_t begin_addr, std::uint64_t size) const;
    NTSTATUS find_pattern(std::uint64_t& pattern_addr,
                          std::uint32_t pid,
                          std::uint64_t begin_addr,
                          std::uint64_t size,
                          const std::string_view& pattern,
                          const std::bitset<256>& mask = std::bitset<256>().set()) const;

private:
    HANDLE m_device{};

    NTSTATUS dispatch_request(aik::internal::request_type req_type, PVOID args) const;
};
}// namespace aik_process::internal
