#pragma once
#include "cuarenta.h"
#include "game_state.h"
#include "rank.h"
#include <vector>
#include <array>
#include <utility>

namespace Bot {

enum class BotType : uint8_t { CHILD, ROBOT, OLDMAN, CHEAT };

struct Move_Eval {
    Cuarenta::Move move;
    float value; // negamax value from the current player's perspective
};

struct Hand_Probability {
    std::array<double, Cuarenta::NUM_CARDS> p1;
};

constexpr Cuarenta::Rank idx_to_rank(size_t idx) {
    return Cuarenta::int_to_rank((static_cast<int>(idx) / (Cuarenta::NUM_CARDS / Cuarenta::NUM_CARD_TYPES) + 1));
};

struct Bot {
    BotType bot_type_;
    Hand_Probability hand_probabilities_;
    Bot(BotType bot_type) : 
        bot_type_{bot_type},
        hand_probabilities_{ std::array<double, Cuarenta::NUM_CARDS>{
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        } } {}
    void update_probabilities(Cuarenta::Move move) {

    }
};

constexpr int heuristic_value(const Cuarenta::Game_State& game_state);
constexpr int deterministic_value(const Cuarenta::Game_State& game_state);

size_t monte_carlo_idx(Hand_Probability prob);

Cuarenta::Move make_move(const Cuarenta::Game_State& game_state, BotType bot);

int minimax(Cuarenta::Game_State& game_state, const int depth);

std::vector<Move_Eval> evaluate_all_moves_mc(
    const Bot& bot,
    Cuarenta::Game_State game_state, 
    const int depth);

Move_Eval choose_best_move(
    const Cuarenta::Game_State& game_state, 
    const int depth);
}