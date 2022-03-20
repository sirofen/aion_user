// aik
#include <aion_write_builder.hpp>
#include <aion_write.hpp>
#include <client_structs.hpp>

aik::aion_write_builder::aion_write_builder()
        : m_aion_write_o(std::make_unique<aion_write>()) {}

void aik::aion_write_builder::set_speed(float speed) {
    m_aion_write_o->m_speed = speed;
}

void aik::aion_write_builder::set_attack_speed(std::uint32_t attack_speed) {
    m_aion_write_o->m_attack_speed = attack_speed;
}

void aik::aion_write_builder::set_pos_x(float pos_x) {
    m_aion_write_o->m_pos_x = pos_x;
}

void aik::aion_write_builder::set_pos_y(float pos_y) {
    m_aion_write_o->m_pos_y = pos_y;
}

void aik::aion_write_builder::set_pos_z(float pos_z) {
    m_aion_write_o->m_pos_z = pos_z;
}

void aik::aion_write_builder::set_gravity(bool gravity) {
    if (gravity) {
        m_aion_write_o->m_gravity = aion_structures::constants::kGravityEnable;
        return;
    }
    m_aion_write_o->m_gravity = aion_structures::constants::kGravityDisable;
}
void aik::aion_write_builder::set_radar(bool radar) {
    if (radar) {
        m_aion_write_o->m_radar = aion_structures::constants::kRadarEnable;
        return;
    }
    m_aion_write_o->m_radar = aion_structures::constants::kRadarDisable;
}

void aik::aion_write_builder::set_console(bool console) {
}

std::unique_ptr<aik::aion_write> aik::aion_write_builder::build() {
    // compiler begs me to move it, even in return statement
    return std::move(m_aion_write_o);
}
