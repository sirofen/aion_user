// aik_process
#include <aik/internal/process.hpp>
#include <aik/internal/driver.hpp>
#include <aik/internal/process_module.hpp>

// utils
#include <aik/utils/utils.hpp>

aik::internal::process::process(std::shared_ptr<aik::internal::driver> driver, const std::wstring& process_name, bool ignore_case)
    : m_driver_o(std::move(driver)) {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }

    m_target_pid = utils::get_process_pid(process_name, ignore_case);
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }
}

bool aik::internal::process::read_memory(const std::uint64_t process_source, void* const target, const std::uint64_t size) const {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }

    return NT_SUCCESS(m_driver_o->read_memory(process_source, m_target_pid, target, size));
}

bool aik::internal::process::write_memory(const std::uint64_t process_target, const void* const source, const std::uint64_t size) const {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }

    return NT_SUCCESS(m_driver_o->write_memory(process_target, m_target_pid, source, size));
}

bool aik::internal::process::find_module(aik::internal::process_module& module, const std::wstring& module_name, bool list_pages) const {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }

    return NT_SUCCESS(m_driver_o->find_module(module, m_target_pid, module_name, list_pages));
}

std::uint64_t aik::internal::process::find_pattern(const std::string_view& pattern, const std::bitset<256>& mask) const {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }

    // scanning all process memory pages
    std::uint64_t res;
    if (!NT_SUCCESS(m_driver_o->find_pattern(res, m_target_pid, 0, _UI64_MAX, pattern, mask))) {
        return 0;
    }
    return res;
}

std::uint64_t aik::internal::process::find_pattern(const aik::internal::process_module& module,
                                                   const std::string_view& pattern,
                                                   const std::bitset<256>& mask) const {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }

    std::uint64_t res;
    if (!NT_SUCCESS(m_driver_o->find_pattern(res,
                                             m_target_pid,
                                             module.get_base_address(),
                                             module.get_size(),
                                             pattern,
                                             mask))) {
        return 0;
    }
    return res;
}

std::uint64_t aik::internal::process::get_base_addr() const {
    if (!m_driver_o) {
        throw std::runtime_error("Driver uninitialised");
    }
    if (!m_target_pid) {
        throw std::runtime_error("Target PID uninitialised");
    }

    std::uint64_t res;
    if (!NT_SUCCESS(m_driver_o->get_base_addr(res, m_target_pid))) {
        return 0;
    }
    return res;
}
