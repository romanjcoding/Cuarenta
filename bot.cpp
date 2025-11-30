#include "bot.h"
#include "game_state.h"
#include "cuarenta.h"
#include "movegen.h"
#include <limits>
#include <algorithm>

namespace Bot {

constexpr int heuristic_value(Cuarenta::Game_State& game_state) {
    return Cuarenta::current_player_state(game_state).score;
};

void monte_carlo_generation();

int minimax(Cuarenta::Game_State& game_state, const int depth) {

    if (depth == 0) { return heuristic_value(game_state); }
    // // if (node is terminal) [ points >= 40 -> value = inf ]
    // // if (hand_player.cards.empty() && hand_opponent.cards.empty()) {return score;}// [ cards exhausted -> redraw hand? ]

    std::vector<Cuarenta::Move> possible_moves { Cuarenta::generate_all_moves(
            game_state.table, 
            Cuarenta::current_player_state(game_state).hand
        ) };

    
    Cuarenta::Game_State updated_state { game_state };
    int value { std::numeric_limits<int>::min() };

    for (Cuarenta::Move move : possible_moves) {
        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        // updated_state.advance_turn();
        value = std::max(value, minimax(updated_state, depth -1));
    }
    return value;
}

}