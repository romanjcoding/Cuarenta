#include "bot.h"

#include "game_state.h"
#include "dynamic_array.h"
#include "cuarenta.h"
#include "movegen.h"
#include "rank.h"

#include <limits>
#include <algorithm>
#include <bit>
#include <utility>
#include <random>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <cmath>
#include <string>

namespace Bot {

constexpr double round_heuristic(const int round, const int num_captured_cards, const Cuarenta::Player player) {
    if (round == 4) { return 0.0; }
    if (player == Cuarenta::Player::P1) {
        switch (round) {
            case 1:
                return 0.7642 * num_captured_cards + 3.1222;
            case 2:
                return 1.5204 + 3.0024 * std::log(1 + std::exp(0.5966 * (static_cast<double>(num_captured_cards) - 6.3061)));
            case 3:
                return 0.6263 + 0.4079 * std::log(1 + std::exp(1.6525 * (static_cast<double>(num_captured_cards) - 11.5297)));
        }
    }
    if (player == Cuarenta::Player::P2) {
        switch (round) {
            case 1:
                return 0.7529 * num_captured_cards + 7.0585;
            case 2:
                return 3.9710 + 0.5644 * std::log(1 + std::exp(1.1499 * (static_cast<double>(num_captured_cards) - 3.2285)));
            case 3:
                return 2.0381 + 0.2713 * std::log(1 + std::exp(1.8338 * (static_cast<double>(num_captured_cards) - 9.1723)));
        }
    }
    throw std::invalid_argument("If I wake up to this I will be sad :(" + std::to_string(round));
}

constexpr double heuristic_value(const Cuarenta::Game_State& game_state) {

    int captured_cards_score { 0 };

    const auto& player { Cuarenta::current_player_state(game_state) };
    const auto& enemy  { Cuarenta::opposing_player_state(game_state) };

    // Scoring rules: 20 cards = 6pts, 22 cards = 8pts, etc.
    if (player.num_captured_cards >= 20) {
            captured_cards_score += 6 + 2 * ((player.num_captured_cards - 20) / 2);
    }

    if (enemy.num_captured_cards >= 20) {
            captured_cards_score -= 6 + 2 * ((enemy.num_captured_cards - 20) / 2);
    }
    
    auto curr_player { (game_state.to_move == Cuarenta::Player::P1) ? Cuarenta::Player::P1 : Cuarenta::Player::P2 };
    const int round { deck_size_to_round(game_state.deck.cards.size()) };
    const double heuristic_score { 
        round_heuristic(round, player.num_captured_cards, curr_player) -
        round_heuristic(round, enemy.num_captured_cards, curr_player)
    };

    return player.score - enemy.score + captured_cards_score + heuristic_score;
}

double minimax(Cuarenta::Game_State& game_state, const int depth) {

    if (Cuarenta::opposing_player_state(game_state).score >= 40) {
        return std::numeric_limits<double>::lowest();
    }

    if (Cuarenta::current_player_state(game_state).hand.cards.empty()) {
        return heuristic_value(game_state);
    }

    if (depth == 0) { return heuristic_value(game_state); }

    const auto available_moves { Cuarenta::generate_all_moves(game_state) };

    double value { std::numeric_limits<double>::lowest() };
    for (size_t i{}; i < available_moves.size(); i++) {

        Cuarenta::Undo undo { Cuarenta::make_move_in_place(game_state, Cuarenta::Move{ available_moves.at(i) } )};

        game_state.advance_turn();
        value = std::max(value, -minimax(game_state, depth - 1));
        game_state.unadvance_turn();

        Cuarenta::undo_move_in_place(game_state, undo);
    }
    return value;
}

std::vector<MoveEval> evaluate_all_moves_mc (
    const Bot& bot,
    Cuarenta::Game_State& game_state, 
    const int depth) {

    const auto available_moves { Cuarenta::generate_all_moves(game_state) };

    if (available_moves.empty()) {
        std::cout << "No available moves to evaluate.\n";
        return {}; 
    }
    std::vector<double> S1(available_moves.size());
    std::vector<double> S2(available_moves.size());

    const auto opp_hand_copy { Cuarenta::opposing_player_state(game_state).hand.cards };

    int NUM_ITER { bot.num_mc_iters_ };

    for (int unused{}; unused < NUM_ITER; unused++) {

        for (auto& rank : Cuarenta::opposing_player_state(game_state).hand.cards) {
            rank = bot.weighted_random_rank();
        }

        for (size_t i{}; i < available_moves.size(); i++) {

            const Cuarenta::Undo undo { Cuarenta::make_move_in_place(game_state, Cuarenta::Move{available_moves.at(i)}) };

            game_state.advance_turn();
            double value = -minimax(game_state, depth - 1);
            game_state.unadvance_turn();

            Cuarenta::undo_move_in_place(game_state, undo);
            
            S1[i] += value;
            S2[i] += value * value;
        }
        Cuarenta::opposing_player_state(game_state).hand.cards = opp_hand_copy;
    }

    std::vector<MoveEval> move_evaluations;
    move_evaluations.reserve(available_moves.size());

    for (size_t i{}; i < available_moves.size(); i++) {
        const double mean { S1[i] / NUM_ITER };
        const double var  { S2[i] / NUM_ITER - mean * mean };
        move_evaluations.emplace_back(MoveEval{ 
            .move = Cuarenta::Move{available_moves.at(i)},
            .eval = mean,
            .std_dev = std::sqrt(var) // clamp to std::max(0.0, std::sqrt(var))?
        });
    }
    return move_evaluations;
}
}