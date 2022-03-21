#pragma once

// C++ STK
#include <memory>
#include <future>


// forward declarations
namespace aik {
class aion_write;
class client_values;
}
namespace aik::internal {
class driver;
class process;
class process_module;
}
namespace aion_structures::cry3dengine {
struct radar;
}
namespace aion_structures::game {
struct entity_inf;
struct entity_pos;
}


namespace aik {
class aik_process {
public:
    aik_process();
    ~aik_process();

    aik_process(const aik_process&)            = delete;
    aik_process& operator=(const aik_process&) = delete;

    aik_process(aik_process&&)                 = delete;
    aik_process& operator=(aik_process&&)      = delete;

    /**
        Keeps trying to attach to a process and find necessary addresses until successful or 'max_retries' limit met.
        If values are already initializing, does nothing.
        @param retry_timeout_ms timeout between retries.
        @param max_retries retries limit, 0 for INFINITE.
        @return TRUE for successful, FALSE if the retry limit has been reached.
    */
    void force_init_async(std::uint32_t retry_timeout_ms = 1000, std::uint32_t max_retries = 0);

    /**
        Reads client values if player name unreachable, calls 'force_init_async()'
        @return client_values filled with zeroes if player wasn't found
    */
    client_values read_client_values();
    /**
        Writes only initialised std::optional 's
    */
    void write_client_values(std::unique_ptr<aion_write> aion_write);

    /**
        Check atomic state
    */
    bool is_process_attached() const;
    /**
        Check atomic state
    */
    bool is_console_pattern_found() const;
    /**
        Check atomic state
    */
    bool is_player_found() const;
    /**
        Check atomic state
    */
    bool is_radar_found() const;
    /**
        Check atomic state
    */
    bool is_currently_launching() const;

    const std::future<bool>& get_launching_future() const;

    void map_driver();

private:
    bool force_init(std::uint32_t retry_timeout_ms = 1000, std::uint32_t max_retries = 0);

    std::unique_ptr<internal::process> m_process;

    std::uint64_t m_aion_proc_console_ptr{};
    aion_structures::cry3dengine::radar* m_aion_proc_radar_ptr{};
    aion_structures::game::entity_inf* m_aion_proc_self_inf_ptr{};
    aion_structures::game::entity_pos* m_aion_proc_self_pos_ptr{};

    std::atomic_bool m_proc_attached;
    std::atomic_bool m_console_found;
    std::atomic_bool m_player_found;
    std::atomic_bool m_radar_found;

    std::atomic_bool m_currently_launching;
    std::future<bool> m_launching_future;

    void print_address_memory(std::uint64_t base, std::uint64_t size, std::uint32_t line_length = 0x10) const;
};
}
