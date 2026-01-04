#include <string_view>

namespace ansi {
inline constexpr std::string_view reset   = "\033[0m";
inline constexpr std::string_view bold    = "\033[1m";
inline constexpr std::string_view dim     = "\033[2m";

inline constexpr std::string_view white   = "\033[37m";
inline constexpr std::string_view gray    = "\033[90m";
inline constexpr std::string_view red     = "\033[31m";
// inline constexpr std::string_view green   = "\033[32m";
inline constexpr std::string_view yellow  = "\033[33m";
inline constexpr std::string_view magenta = "\033[35m";
} // namespace ansi