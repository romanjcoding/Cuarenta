#include "cuarenta.h"
#include <limits>
#include <algorithm>

namespace Bot {

    int heuristic_value(Cuarenta::Table& table);

    void monte_carlo_generation();

    std::vector<Cuarenta::Move> generate_all_moves(
        Cuarenta::Hand& hand,
        Cuarenta::Table& table) {
            
            std::vector<Cuarenta::Move> moves;
            moves.reserve(Cuarenta::MAX_MOVES_PER_BOARD);

    }

    int minimax(int depth,
                Cuarenta::Hand& hand_player, 
                Cuarenta::Hand& hand_opponenet, 
                Cuarenta::Table& table,
                bool maximizing_player) {

        if (depth == 0) { return heuristic_value(table); }
        // if (node is terminal) [ points >= 40 -> value = inf ]
        // if (node is terminal) [ cards exhausted -> redraw hand? ]

        if (maximizing_player) {
            int value { std::numeric_limits<int>::min() };
            
        }

        else {

        }
        ;
        ;
        ;
        return;
    } 
}