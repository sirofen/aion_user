// aik_process
#include <aik/internal/page.hpp>
#include <aik/internal/process_module.hpp>

// C++ STL
#include <stdexcept>

aik::internal::process_module::process_module(const std::uint64_t base_address,
                                    const std::uint32_t module_size,
                                    std::vector<aik::internal::page> memory_pages)
    : m_addr(base_address)
    , m_size(module_size)
    , m_mem_pages(std::move(memory_pages)) {}

std::uint64_t aik::internal::process_module::get_base_address() const {
    if (!m_addr) {
        throw std::runtime_error("Called uninitialised value");
    }
    return m_addr;
}

std::uint32_t aik::internal::process_module::get_size() const {
    if (!m_size) {
        throw std::runtime_error("Called uninitialised value");
    }
    return m_size;
}

std::vector<aik::internal::page> aik::internal::process_module::get_pages() const {
    if (m_mem_pages.empty()) {
        throw std::runtime_error("Called uninitialised value");
    }
    return m_mem_pages;
}
