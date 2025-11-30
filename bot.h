#pragma once
#include "cuarenta.h"
#include "game_state.h"
#include <vector>

namespace Bot {

constexpr int heuristic_value(Cuarenta::Game_State& game_state);
void monte_carlo_generation();

int minimax(Cuarenta::Game_State&, const int depth);

}