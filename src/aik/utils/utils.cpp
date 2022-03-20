#include <aik/utils/utils.hpp>

std::uint64_t utils::find_pattern(const char* const buffer, const std::uint64_t base, const std::uint64_t size, const std::string_view& pattern, const std::bitset<256>& mask) {
    const auto pattern_i_max = pattern.size() - 1;

    std::uint64_t pattern_i = 0;
    for (std::uint64_t i = 0; i < size; i++) {
        if (pattern_i == pattern_i_max) {
            return base + i - pattern_i;
        }
        if (!mask.test(pattern_i_max - pattern_i) || buffer[i] == pattern[pattern_i]) {
            pattern_i++;
            continue;
        }
        pattern_i = 0;
    }
    return 0;
}
