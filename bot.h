#pragma once
#include "cuarenta.h"
#include "game_state.h"
#include <vector>
#include <utility>

namespace Bot {

enum class BotType : uint8_t { CHILD, ROBOT, OLDMAN, CHEAT };

struct Move_Eval {
    Cuarenta::Move move;
    int value; // negamax value from the current player's perspective
};

struct Hand_Probability {
    std::array<double, Cuarenta::NUM_CARDS> p1;
};

constexpr int heuristic_value(const Cuarenta::Game_State& game_state);
constexpr int deterministic_value(const Cuarenta::Game_State& game_state);

size_t monte_carlo_idx(std::array<double, Cuarenta::NUM_CARDS> prob);


Cuarenta::Move make_move(const Cuarenta::Game_State& game_state, BotType bot);

int minimax(Cuarenta::Game_State& game_state, const int depth);

std::vector<Move_Eval> evaluate_all_moves(
    const Cuarenta::Game_State& game_state, 
    const int depth);

Move_Eval choose_best_move(
    const Cuarenta::Game_State& game_state, 
    const int depth);
}