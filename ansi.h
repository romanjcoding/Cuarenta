#pragma once
#include <array>
#include <string_view>
#include <algorithm>

namespace ansi {

inline constexpr std::string_view reset   = "\033[0m";
inline constexpr std::string_view magenta = "\033[35m";
inline constexpr std::string_view bold    = "\033[1m";
inline constexpr std::string_view dim     = "\033[2m";

inline constexpr std::string_view white   = "\033[37m";
inline constexpr std::string_view gray    = "\033[90m";

// Format: \033[38;2;R;G;Bm
inline constexpr std::array<std::string_view, 10> yellow_to_purple_10 = {
    "\x1b[38;5;226m", // yellow
    "\x1b[38;5;220m", // yellow-orange
    "\x1b[38;5;214m", // orange
    "\x1b[38;5;208m", // orange-red
    "\x1b[38;5;202m", // red-orange
    "\x1b[38;5;196m", // red
    "\x1b[38;5;197m", // red -> magenta
    "\x1b[38;5;198m",
    "\x1b[38;5;199m",
    "\x1b[38;5;201m"  // purple/magenta
};

inline constexpr std::string_view yellow = yellow_to_purple_10.front();
inline constexpr std::string_view red    = yellow_to_purple_10[5];

constexpr std::string_view waterfall(size_t i) {
    return yellow_to_purple_10[std::min(i, yellow_to_purple_10.size() - 1)];
}

} // namespace ansi
