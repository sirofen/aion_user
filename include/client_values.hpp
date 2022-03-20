#pragma once

// C++ STL
#include <memory>
#include <string_view>

// forward declarations
namespace aion_structures::game {
struct entity_inf;
struct entity_pos;
}

namespace aik {
class client_values {
    friend class aik_process;
public:
    client_values(const client_values&)                = delete;
    client_values operator=(const client_values&)      = delete;

    client_values(client_values&&)            noexcept = default;
    client_values& operator=(client_values&&) noexcept = default;

    [[nodiscard]] std::uint8_t get_level() const;
    [[nodiscard]] std::string_view get_name() const;
    [[nodiscard]] std::uint32_t get_attack_speed() const;
    [[nodiscard]] float get_speed() const;
    [[nodiscard]] bool get_gravity() const;
    [[nodiscard]] float get_pos_x() const;
    [[nodiscard]] float get_pos_y() const;
    [[nodiscard]] float get_pos_z() const;

private:
    client_values(std::unique_ptr<::aion_structures::game::entity_inf> entity_inf,
                  std::unique_ptr<::aion_structures::game::entity_pos> entity_pos);

    std::unique_ptr<::aion_structures::game::entity_inf> m_entity_inf;
    std::unique_ptr<::aion_structures::game::entity_pos> m_entity_pos;
};
}
