#pragma once

// C++ STL
#include <optional>
#include <memory>

// forward declaration
namespace aik {
class aion_write;
}

namespace aik {
class aion_write_builder {
public:
    aion_write_builder();

    void set_speed(float speed);
    void set_attack_speed(std::uint32_t attack_speed);

    void set_pos_x(float pos_x);
    void set_pos_y(float pos_y);
    void set_pos_z(float pos_z);

    void set_gravity(bool gravity);
    void set_radar(bool radar);
    void set_console(bool console);

    [[nodiscard]] std::unique_ptr<aion_write> build();
private:
    std::unique_ptr<aion_write> m_aion_write_o;
};
}