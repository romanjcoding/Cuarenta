#include "bot.h"
#include "cuarenta.h"
#include "movegen.h"
#include <limits>
#include <algorithm>

namespace Bot {

int heuristic_value(Cuarenta::Table& table);

void monte_carlo_generation();

int minimax(Cuarenta::Hand& hand_player, 
            Cuarenta::Hand& hand_opponent, 
            Cuarenta::Table& table,
            int depth,
            int score,
            bool color) {

    // if (depth == 0) { return heuristic_value(table); }
    if (depth == 0) { return score; }
    // if (node is terminal) [ points >= 40 -> value = inf ]
    // if (hand_player.cards.empty() && hand_opponent.cards.empty()) {return score;}// [ cards exhausted -> redraw hand? ]

    std::vector<Cuarenta::Move> possible_moves { Cuarenta::generate_all_moves(table, hand_player) };
    int value { std::numeric_limits<int>::min() };

    for (Cuarenta::Move move : Cuarenta::generate_all_moves(table, hand_player)) {
        std::bitset<16> x{Cuarenta::to_u16(move.table_targets)};
        std::cout << x << '\n';
        value = std::max(value, play_card(move, hand_player, table));
    }
    return value;
}



}