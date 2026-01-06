#pragma once
#include "ansi.h"
#include "bot.h"

#include <thread>
#include <string_view>
#include <optional>
#include <iostream>
#include <vector>

namespace cli {

constexpr void clear_screen() {
    #if defined(_WIN32)
        std::system("cls");
    #else
        std::cout << "\033[2J\033[H" << std::flush;
    #endif
}

static constexpr int TABLE_CAPACITY { 11 }; // 10 (max capacity) + 1 (for intermediate captures)
static constexpr int CARD_CELL_W    { 8 }; 
static constexpr int TABLE_INNER_W  { TABLE_CAPACITY * CARD_CELL_W };
static constexpr int CARD_LINES_H   { 5 };


constexpr void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

struct RenderOpts {
    std::vector<size_t> table_idx_to_highlight { };
    std::string_view table_color      { ansi::reset };

    std::optional<size_t> bottom_hi_index {};
    std::string_view bottom_color    { ansi::reset };

    std::optional<size_t> top_hi_index {};
    std::string_view top_color       { ansi::reset };

    bool error_flash { false };

    std::string banner {};
    std::string_view banner_color    { ansi::magenta };
    bool banner_bold { true };
};

enum class View {
    ByTurn,
    P1Fixed
};

struct InputData {
    std::optional<Bot::Bot> bot        { std::nullopt };
    std::optional<Cuarenta::Move> move { std::nullopt };

    bool play_bot   { false };
    bool play_human { false };
    bool bot_stats  { false };
    
    bool help { false };
    bool quit { false };
    bool err  { false };
};

enum class State {
    Menu,
    MenuHelp,
    Game,
    GameHelp,
};

int run_cli();

} // namespace cli
