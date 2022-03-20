#pragma once
// aik
#include <client_values.hpp>
#include <client_structs.hpp>

aik::client_values::client_values(std::unique_ptr<::aion_structures::game::entity_inf> entity_inf, std::unique_ptr<::aion_structures::game::entity_pos> entity_pos)
    : m_entity_inf(std::move(entity_inf)),
      m_entity_pos(std::move(entity_pos)) {}

std::uint8_t aik::client_values::get_level() const {
    return m_entity_inf->level;
}
std::string_view aik::client_values::get_name() const {
    return m_entity_inf->name;
}
std::uint32_t aik::client_values::get_attack_speed() const {
    return m_entity_inf->attack_speed;
}
float aik::client_values::get_speed() const {
    return m_entity_inf->speed;
}
bool aik::client_values::get_gravity() const {
    return m_entity_inf->gravity;
}

float aik::client_values::get_pos_x() const {
    return m_entity_pos->x;
}
float aik::client_values::get_pos_y() const {
    return m_entity_pos->y;
}
float aik::client_values::get_pos_z() const {
    return m_entity_pos->z;
}
