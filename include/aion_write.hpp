#pragma once
#include <optional>

// forward declaration
namespace aik {
class aion_write_builder;
}

namespace aik {
class aion_write {
    friend class aion_write_builder;
public:
    aion_write() = default;

    [[nodiscard]] const std::optional<float>& get_speed() const;
    [[nodiscard]] const std::optional<std::uint32_t>& get_attack_speed() const;
    [[nodiscard]] const std::optional<float>& get_pox_x() const;
    [[nodiscard]] const std::optional<float>& get_pos_y() const;
    [[nodiscard]] const std::optional<float>& get_pos_z() const;
    [[nodiscard]] const std::optional<char>& get_gravity() const;
    [[nodiscard]] const std::optional<char>& get_radar() const;
    [[nodiscard]] const std::optional<char>& get_console() const;

private:
    std::optional<float> m_speed;
    std::optional<std::uint32_t> m_attack_speed;

    std::optional<float> m_pos_x;
    std::optional<float> m_pos_y;
    std::optional<float> m_pos_z;

    std::optional<char> m_gravity;
    std::optional<char> m_radar;
    std::optional<char> m_console;
};
}
