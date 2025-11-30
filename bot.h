#pragma once
#include "cuarenta.h"
#include <vector>

namespace Bot {

int heuristic_value(Cuarenta::Table& table);
void monte_carlo_generation();


int minimax(Cuarenta::Hand& hand_player, 
            Cuarenta::Hand& hand_opponent, 
            Cuarenta::Table& table,
            int depth,
            int score);

}