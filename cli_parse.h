#pragma once
#include "bot.h"
#include "game_state.h"
#include <optional>

namespace cli {

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

std::optional<Cuarenta::Rank> try_parse_rank_token(const std::string& token);
InputData read_input();


} // namespace cli