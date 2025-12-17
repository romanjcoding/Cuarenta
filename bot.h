#pragma once
#include "cuarenta.h"
#include "game_state.h"
#include <vector>
#include <utility>

namespace Bot {

constexpr int heuristic_value(const Cuarenta::Game_State& game_state);
constexpr int deterministic_value(const Cuarenta::Game_State& game_state);

size_t monte_carlo_idx(std::array<double, Cuarenta::NUM_CARDS> prob);


Cuarenta::Move make_move(const Cuarenta::Game_State& game_state, BotType bot);
int minimax(Cuarenta::Game_State& game_state, const int depth);
std::pair<int, Cuarenta::Move> choose_best_move(const Cuarenta::Game_State& game_state, const int depth);

}