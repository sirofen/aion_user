// aik
#include <aion_write.hpp>

const std::optional<float>& aik::aion_write::get_speed() const {
    return m_speed;
}
const std::optional<std::uint32_t>& aik::aion_write::get_attack_speed() const {
    return m_attack_speed;
}
const std::optional<float>& aik::aion_write::get_pox_x() const {
    return m_pos_x;
}
const std::optional<float>& aik::aion_write::get_pos_y() const {
    return m_pos_y;
}
const std::optional<float>& aik::aion_write::get_pos_z() const {
    return m_pos_z;
}
const std::optional<char>& aik::aion_write::get_gravity() const {
    return m_gravity;
}
const std::optional<char>& aik::aion_write::get_radar() const {
    return m_radar;
}
const std::optional<char>& aik::aion_write::get_console() const {
    return m_console;
}
