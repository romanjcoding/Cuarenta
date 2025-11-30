#include "bot.h"
#include "game_state.h"
#include "cuarenta.h"
#include "movegen.h"
#include <limits>
#include <algorithm>
#include <bit>

namespace Bot {

constexpr int heuristic_value(const Cuarenta::Game_State& game_state) {
    return Cuarenta::current_player_state(game_state).score - 
           Cuarenta::opposing_player_state(game_state).score;
};

constexpr int deterministic_value(const Cuarenta::Game_State& game_state) {

    int captured_cards_score { 0 };

    for (const Cuarenta::Player_State& player_state : game_state.players) {

        // Scoring rules: 20 cards = 6pts, 22 cards = 8pts, etc.
        if (player_state.score >= 20) {
            captured_cards_score = 6 + (player_state.score - 20) / 2;
        }
    }

    return Cuarenta::current_player_state(game_state).score - 
           Cuarenta::opposing_player_state(game_state).score + 
           captured_cards_score;
}

void monte_carlo_generation();

int minimax(Cuarenta::Game_State& game_state, const int depth, const std::string& line_so_far) {

    if (Cuarenta::opposing_player_state(game_state).score >= 40) {
        int h { std::numeric_limits<int>::min() };
        std::cout << line_so_far << " => " << h << '\n';
        return h;
    }

    // [ cards exhausted -> redraw hand? ]
    if (Cuarenta::current_player_state(game_state).hand.cards.empty()) {
            int h { deterministic_value(game_state) };
            std::cout << line_so_far << " => " << h << '\n';
            return h;
    }

    if (depth == 0) { 
        int h { heuristic_value(game_state) };
        std::cout << line_so_far << " => " << h << '\n';
        return h;
    }

    std::vector<Cuarenta::Move> possible_moves { Cuarenta::generate_all_moves(
            game_state.table, 
            Cuarenta::current_player_state(game_state).hand
        ) };

    if (possible_moves.size() == 0) { 
        std::cout << "ERROR: size of possible moves is 0." << '\n';
    }
    
    int value { std::numeric_limits<int>::min() };

    for (const Cuarenta::Move& move : possible_moves) {

        uint16_t mask = to_u16(move.targets_mask);
        int msb_index = Cuarenta::NUM_RANK_BITS - std::countl_zero(mask);
        Cuarenta::Rank played_card { Cuarenta::int_to_rank(msb_index) };

        std::string new_line {
            line_so_far + " " + Cuarenta::rank_to_str(played_card)
        };

        if (!std::has_single_bit(mask)) {
            new_line += '(';

            // Strip off the MSB to get “everything but the MSB”
            Cuarenta::RankMask additions_mask {
                move.targets_mask & ~Cuarenta::to_mask(played_card)
            };

            bool first = true;
            for (int bit = 1; bit <= rank_to_int(Cuarenta::Rank::King); bit++) {

                Cuarenta::Rank rank        { Cuarenta::int_to_rank(bit) };
                Cuarenta::RankMask bitmask { Cuarenta::to_mask(rank) };

                if ((additions_mask & bitmask) != Cuarenta::RankMask{}) {
                    if (!first) {
                        new_line += '+';
                    }
                    new_line += Cuarenta::rank_to_str(rank);
                    first = false;
                }
            }
            new_line += ')';
        }

        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        updated_state.advance_turn();
        value = std::max(value, -minimax(updated_state, depth - 1, new_line));
    }
    return value;
}

}