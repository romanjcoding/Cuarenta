#pragma once
#include "cuarenta.h"
#include <vector>

namespace Bot {

int heuristic_value(Cuarenta::Table& table);
void monte_carlo_generation();

std::vector<Cuarenta::Move> generate_all_moves(
    Cuarenta::Hand& hand,
    Cuarenta::Table& table);

int minimax(int depth,
            Cuarenta::Hand& hand_player, 
            Cuarenta::Hand& hand_opponenet, 
            Cuarenta::Table& table,
            bool maximizing_player);

}