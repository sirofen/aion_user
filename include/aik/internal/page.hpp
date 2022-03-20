#pragma once

// C++ STL
#include <cstdint>

namespace aik::internal {
class page {
public:
    page() = default;
    page(std::uint64_t addr, std::uint64_t size);

    [[nodiscard]] std::uint64_t get_address() const;
    [[nodiscard]] std::uint64_t get_size() const;

private:
    std::uint64_t m_addr{};
    std::uint64_t m_size{};
};
}// namespace aik_process
