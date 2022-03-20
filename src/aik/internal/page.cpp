// aik_process
#include <aik/internal/page.hpp>

aik::internal::page::page(const std::uint64_t addr, const std::uint64_t size)
    : m_addr(addr)
    , m_size(size) {}

std::uint64_t aik::internal::page::get_address() const {
    return m_addr;
}

std::uint64_t aik::internal::page::get_size() const {
    return m_size;
}
