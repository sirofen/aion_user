#pragma once

// C++ STL
#include <vector>

// forward declaration
namespace aik::internal {
class page;
}

namespace aik::internal {
class process_module {
public:
    process_module()                                     = default;
    ~process_module()                                    = default;

    process_module(const process_module&)                = delete;
    process_module& operator=(const process_module&)     = delete;

    process_module(process_module&&)            noexcept = default;
    process_module& operator=(process_module&&) noexcept = default;


    process_module(std::uint64_t base_address,
                   std::uint32_t module_size,
                   std::vector<page> memory_pages);

    [[nodiscard]] std::uint64_t get_base_address() const;
    [[nodiscard]] std::uint32_t get_size() const;

    [[nodiscard]] std::vector<page> get_pages() const;

private:
    std::uint64_t m_addr{};
    std::uint32_t m_size{};

    std::vector<page> m_mem_pages;
};
}// namespace aik_process
