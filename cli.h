#pragma once
#include "ansi.h"
#include "bot.h"

#include <thread>
#include <string_view>
#include <optional>
#include <iostream>
#include <vector>

namespace cli {

constexpr void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

enum class View {
    ByTurn,
    P1Fixed
};

enum class State {
    Menu,
    MenuHelp,
    BotSelect,
    PlayingHuman,
    PlayingBot,
    PlayingHelp,
    Quit
};

int run_cli();

} // namespace cli
