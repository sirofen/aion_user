// aik
#include <aik_process.hpp>
#include <aik/internal/process.hpp>
#include <aik/internal/driver.hpp>
#include <aik/internal/process_module.hpp>
#include <aik/internal/page.hpp>
#include <aion_write.hpp>
#include <client_structs.hpp>
#include <client_values.hpp>
#include <aik/utils/run_pe.hpp>
#include <aik/resources/aik_map_resource.hpp>

// aik utils
#include <aik/utils/utils.hpp>

// winapi
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C++ STL
#include <stdexcept>
#include <iostream>

aik::aik_process::aik_process()  = default;
aik::aik_process::~aik_process() = default;

bool aik::aik_process::force_init(uint32_t retry_timeout_ms, std::uint32_t max_retries) {
    using namespace aion_structures;
    m_currently_launching.store(true, std::memory_order_release);

    std::shared_ptr<aik::internal::driver> drv_shd;
    try {
        drv_shd = std::make_shared<aik::internal::driver>();
    } catch (const std::runtime_error& e) {
        std::printf("%s\n", e.what());
        return false;
    }

    std::uint32_t step = 1;

    if (!max_retries) {
        max_retries = 1;
        step = 0;
    }

    aik::internal::process_module game_dll;
    aik::internal::process_module cry_engine_dll;
    // cry_engine sz 0x002a3000
    // game sz 0x01b57000

    for (;max_retries; max_retries -= step) {
        try {
            m_proc_attached.store(false, std::memory_order_release);
            m_console_found.store(false, std::memory_order_release);
            m_radar_found.store(false, std::memory_order_release);
            m_player_found.store(false, std::memory_order_release);

            m_process = std::move(std::make_unique<aik::internal::process>(drv_shd, aion_structures::constants::kProcessName, true));
            m_proc_attached.store(true, std::memory_order_release);

            while (true) {
                m_aion_proc_console_ptr = m_process->find_pattern(reinterpret_cast<const char*>(patterns::console::pattern),
                                                                  std::bitset<256>(patterns::console::mask));
                if (m_aion_proc_console_ptr) {
                    m_aion_proc_console_ptr += patterns::console::disable_console_offset;
                    m_console_found.store(true, std::memory_order_release);
                    break;
                }
                Sleep(retry_timeout_ms);
            }
            break;
        } catch (const std::runtime_error& e) {
            std::printf("%s\n", e.what());
            Sleep(retry_timeout_ms);
        }
    }

    for (;max_retries; max_retries -= step) {
        try {
            game::unk1* p_unk1;
            game::unk2* p_unk2;
            game::unk3* p_unk3;
            game::entity* p_self;

            for (; !m_process->find_module(game_dll, aion_structures::constants::kGameModuleName); Sleep(retry_timeout_ms)) {}
            for (; !m_process->find_module(cry_engine_dll, aion_structures::constants::kCry3dEngineModuleName); Sleep(retry_timeout_ms)) {}

            if (!m_process->read_memory(
                    cry_engine_dll.get_base_address() + offsetof(cry3dengine::cry3dengine_module, cry3dengine::cry3dengine_module::radar),
                    &m_aion_proc_radar_ptr,
                    8) || !m_aion_proc_radar_ptr) {
                Sleep(retry_timeout_ms);
                continue;
            }

            cry3dengine::radar radar;
            if (!m_process->read_memory(
                    reinterpret_cast<std::uint64_t>(m_aion_proc_radar_ptr) + offsetof(cry3dengine::radar, cry3dengine::radar::init),
                    &radar.init,
                    sizeof(cry3dengine::radar) - offsetof(cry3dengine::radar, cry3dengine::radar::init))) {
                Sleep(retry_timeout_ms);
            }
            if (radar.init == 0x0 || radar.init == 0x5) {
                m_radar_found.store(true, std::memory_order_release);
            } else {
                continue;
            }

            if (!m_process->read_memory(game_dll.get_base_address() + offsetof(game::game_module, game::game_module::unk1), &p_unk1, 8) ||
                   !p_unk1) {
                Sleep(retry_timeout_ms);
                continue;
            }
            if (!m_process->read_memory(reinterpret_cast<std::uint64_t>(p_unk1) + offsetof(game::unk1, game::unk1::unk2), &p_unk2, 8) ||
                   !p_unk2) {
                Sleep(retry_timeout_ms);
                continue;
            }
            if (!m_process->read_memory(reinterpret_cast<std::uint64_t>(p_unk2) + offsetof(game::unk2, game::unk2::unk3), &p_unk3, 8) ||
                   !p_unk3) {
                Sleep(retry_timeout_ms);
                continue;
            }
            if (!m_process->read_memory(reinterpret_cast<std::uint64_t>(p_unk3) + offsetof(game::unk3, game::unk3::self_player), &p_self, 8) ||
                   !p_self) {
                Sleep(retry_timeout_ms);
                continue;
            }

            game::entity self;
            if (!m_process->read_memory(
                    reinterpret_cast<std::uint64_t>(p_self) + offsetof(game::entity, game::entity::pos),
                    &self.pos,
                    sizeof(game::entity) - offsetof(game::entity, game::entity::pos))) {
                Sleep(retry_timeout_ms);
                continue;
            }

            m_aion_proc_self_pos_ptr = self.pos;
            m_aion_proc_self_inf_ptr = self.inf;

            game::entity_inf inf;
            if (!m_process->read_memory(
                    reinterpret_cast<std::uint64_t>(self.inf) + offsetof(game::entity_inf, game::entity_inf::level),
                    &inf.level,
                    sizeof(game::entity_inf) - offsetof(game::entity_inf, game::entity_inf::level))) {
                Sleep(retry_timeout_ms);
                continue;
            }
            if (std::strlen(inf.name) > 0) {
                m_player_found.store(true, std::memory_order_release);
                m_currently_launching.store(false, std::memory_order_release);
                return true;
            }

        } catch (const std::runtime_error& e) {
            std::printf("%s\n", e.what());
            Sleep(retry_timeout_ms);
        }
    }
    m_currently_launching.store(false, std::memory_order_release);
    return false;
}

void aik::aik_process::force_init_async(std::uint32_t retry_timeout_ms, std::uint32_t max_retries) {
    if (m_currently_launching.load(std::memory_order_acquire)) {
        return;
    }
    m_launching_future = std::async(std::launch::async, [=]{return force_init(retry_timeout_ms, max_retries);});
}

aik::client_values aik::aik_process::read_client_values() {
    using namespace aion_structures;
    game::entity_pos pos{};
    game::entity_inf inf{};
    if (!m_player_found.load(std::memory_order_acquire)) {
        force_init_async();
        return {std::make_unique<game::entity_inf>(inf), std::make_unique<game::entity_pos>(pos)};
    }
    m_process->read_memory(
            reinterpret_cast<std::uint64_t>(m_aion_proc_self_pos_ptr) + offsetof(game::entity_pos, game::entity_pos::x),
            &pos.x,
            sizeof(game::entity_pos) - offsetof(game::entity_pos, game::entity_pos::x));

    m_process->read_memory(
            reinterpret_cast<std::uint64_t>(m_aion_proc_self_inf_ptr) + offsetof(game::entity_inf, game::entity_inf::level),
            &inf.level,
            sizeof(game::entity_inf) - offsetof(game::entity_inf, game::entity_inf::level));
    if (!std::strlen(inf.name)) {
        m_player_found.store(false, std::memory_order_release);
        force_init_async();
    }
    return {std::make_unique<game::entity_inf>(inf), std::make_unique<game::entity_pos>(pos)};
}

void aik::aik_process::write_client_values(std::unique_ptr<aion_write> aion_write) {
    using namespace aion_structures;
    if (!is_process_attached()) {
        return;
    }
    if (is_console_pattern_found() && aion_write->get_console()) {
        m_process->write_memory(m_aion_proc_console_ptr, &*aion_write->get_console(), sizeof(*aion_write->get_console()));
    }
    if (is_radar_found() && aion_write->get_radar()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_radar_ptr) + offsetof(cry3dengine::radar, cry3dengine::radar::init),
                                &*aion_write->get_radar(),
                                sizeof(*aion_write->get_radar()));
    }
    if (!is_player_found()) {
        return;
    }

    if (aion_write->get_attack_speed()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_self_inf_ptr) + offsetof(game::entity_inf, game::entity_inf::attack_speed),
                                &*aion_write->get_attack_speed(),
                                sizeof(*aion_write->get_attack_speed()));
    }
    if (aion_write->get_speed()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_self_inf_ptr) + offsetof(game::entity_inf, game::entity_inf::speed),
                                &*aion_write->get_speed(),
                                sizeof(*aion_write->get_speed()));
    }
    if (aion_write->get_gravity()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_self_inf_ptr) + offsetof(game::entity_inf, game::entity_inf::gravity),
                                &*aion_write->get_gravity(),
                                sizeof(*aion_write->get_gravity()));
    }
    if (aion_write->get_pox_x()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_self_pos_ptr) + offsetof(game::entity_pos, game::entity_pos::x),
                                &*aion_write->get_pox_x(),
                                sizeof(*aion_write->get_pox_x()));
    }
    if (aion_write->get_pos_y()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_self_pos_ptr) + offsetof(game::entity_pos, game::entity_pos::y),
                                &*aion_write->get_pos_y(),
                                sizeof(*aion_write->get_pos_y()));
    }
    if (aion_write->get_pos_z()) {
        m_process->write_memory(reinterpret_cast<std::uint64_t>(m_aion_proc_self_pos_ptr) + offsetof(game::entity_pos, game::entity_pos::z),
                                &*aion_write->get_pos_z(),
                                sizeof(*aion_write->get_pos_z()));
    }

}

bool aik::aik_process::is_process_attached() const {
    return m_proc_attached.load(std::memory_order_acquire);
}
bool aik::aik_process::is_console_pattern_found() const {
    return m_console_found.load(std::memory_order_acquire);
}
bool aik::aik_process::is_player_found() const {
    return m_player_found.load(std::memory_order_acquire);
}
bool aik::aik_process::is_radar_found() const {
    return m_radar_found.load(std::memory_order_acquire);
}
bool aik::aik_process::is_currently_launching() const {
    return m_currently_launching.load(std::memory_order_acquire);
}
const std::future<bool>& aik::aik_process::get_launching_future() const {
    return m_launching_future;
}
void aik::aik_process::map_driver() {
    if (is_process_attached()) {
        return;
    }
    run_pe(aik_map::pe);
}

void aik::aik_process::print_address_memory(const std::uint64_t base, const std::uint64_t size, const std::uint32_t line_length) const {
    char* buf = new char[size];
    if (!m_process->read_memory(base, buf, size)) {
        delete[] buf;
        std::cout << "print_address_memory err" << std::endl;
        return;
    }
    _print_bytes_line(std::string_view(buf, size), base, line_length, ' ');
    delete[] buf;
}
