#include "bot.h"
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

        std::vector<int> possible_summands;
        possible_summands.reserve(Cuarenta::MAX_MOVES_PER_BOARD - 5);

        // Every non-addition move is playable, so add every card in hand
        // for (const Cuarenta::Card& card : table.cards) {
        //     moves.emplace_back(Cuarenta::Move{card}); 
        // }

        // 1 2 J 6 7
        // 1
        // 1 3 2
        // 1 3 2
        // 1 3 2 7 6
        // 1 3 2 7 6 7

        // Determine possible additions
        ;
    return {};
}

int minimax(int depth,
            Cuarenta::Hand& hand_player, 
            Cuarenta::Hand& hand_opponenet, 
            Cuarenta::Table& table,
            bool maximizing_player) {

    // if (depth == 0) { return heuristic_value(table); }
    // if (node is terminal) [ points >= 40 -> value = inf ]
    // if (node is terminal) [ cards exhausted -> redraw hand? ]

    if (maximizing_player) {
        // int value { std::numeric_limits<int>::min() };
    }

    else {

    }
    ;
    ;
    ;
    return 0;
} 

}