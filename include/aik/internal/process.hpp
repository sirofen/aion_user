#pragma once

// C++ STL
#include <memory>
#include <string>
#include <bitset>

// forward declarations
namespace aik::internal {
class driver;
class process_module;
}

namespace aik::internal {
class process {
public:
    ~process()                             = default;

    process(const process&)                = delete;
    process& operator=(const process&)     = delete;

    process(process&&)            noexcept = default;
    process& operator=(process&&) noexcept = default;

    explicit process(std::shared_ptr<aik::internal::driver> driver, const std::wstring& process_name, bool ignore_case);

    bool read_memory(std::uint64_t process_source_addr, void* target, std::uint64_t size) const;
    bool write_memory(std::uint64_t process_target_addr, const void* source, std::uint64_t size) const;

    bool find_module(aik::internal::process_module& module, const std::wstring& module_name, bool list_pages = false) const;

    [[nodiscard]] std::uint64_t find_pattern(const std::string_view& pattern, const std::bitset<256>& mask) const;
    [[nodiscard]] std::uint64_t find_pattern(const aik::internal::process_module& module,
                                             const std::string_view& pattern,
                                             const std::bitset<256>& mask = std::bitset<256>().set()) const;

    [[nodiscard]] std::uint64_t get_base_addr() const;

private:
    std::shared_ptr<aik::internal::driver> m_driver_o;

    std::uint32_t m_target_pid{};
};
}// namespace aik_process
