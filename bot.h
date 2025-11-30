#pragma once
#include "cuarenta.h"
#include "game_state.h"
#include <vector>

namespace Bot {

constexpr int heuristic_value(const Cuarenta::Game_State& game_state);
constexpr int deterministic_value(const Cuarenta::Game_State& game_state);

void monte_carlo_generation();

int minimax(Cuarenta::Game_State& game_state, const int depth, const std::string& line_so_far);

}