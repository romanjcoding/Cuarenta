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
#include <cassert>
#include <map>

namespace Bot {

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
    
    return player.score - enemy.score + captured_cards_score;
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

// not exposed in header
std::pair<MoveEval, std::vector<MoveEval>> get_evaluation_data (
    const Bot& bot,
    Cuarenta::Game_State& game, 
    const int depth) {

    const auto available_moves { Cuarenta::generate_all_moves(game) };

    if (available_moves.empty()) {
        std::cout << "No available moves to evaluate.\n";
        return {};
    }
    std::vector<double> S1(available_moves.size());
    std::vector<double> S2(available_moves.size());

    const auto opp_hand_copy { Cuarenta::opposing_player_state(game).hand.cards };

    int NUM_ITER { bot.num_mc_iters_ };

    for (int unused{}; unused < NUM_ITER; unused++) {

        for (auto& rank : Cuarenta::opposing_player_state(game).hand.cards) {
            rank = bot.weighted_random_rank();
        }

        for (size_t i{}; i < available_moves.size(); i++) {

            const Cuarenta::Undo undo { Cuarenta::make_move_in_place(game, Cuarenta::Move{available_moves.at(i)}) };
            game.advance_turn();
            double value = -minimax(game, depth - 1);
            game.unadvance_turn();
            Cuarenta::undo_move_in_place(game, undo);
            
            S1[i] += value;
            S2[i] += value * value;
        }
        Cuarenta::opposing_player_state(game).hand.cards = opp_hand_copy;
    }

    std::vector<MoveEval> move_evaluations;
    MoveEval best_move { .eval = std::numeric_limits<double>::lowest(), 
                         .move = {},
                         .std_dev = {}}; 
    move_evaluations.reserve(available_moves.size());

    for (size_t i{}; i < available_moves.size(); i++) {
        const double mean { S1[i] / NUM_ITER };
        const double var  { S2[i] / NUM_ITER - mean * mean };
        move_evaluations.emplace_back(MoveEval{ 
            .move = Cuarenta::Move{available_moves.at(i)},
            .eval = mean,
            .std_dev = std::sqrt(std::max(0.0, var))
        });
        if (mean > best_move.eval) { best_move = move_evaluations.back(); }
    }
    return std::pair<MoveEval, std::vector<MoveEval>>{best_move, move_evaluations};
}

Cuarenta::Move choose_best_move(const Bot& bot, Cuarenta::Game_State game, const int depth) {
    return get_evaluation_data(bot, game, depth).first.move;
}

std::vector<MoveEval> evaluate_all_moves(const Bot& bot, Cuarenta::Game_State game, const int depth) {
    return get_evaluation_data(bot, game, depth).second;
}

std::pair<MoveEval, bool> determine_if_confident (
    const Bot& bot,
    Cuarenta::Game_State& game, 
    const int depth) {

    const auto available_moves { Cuarenta::generate_all_moves(game) };

    if (available_moves.empty()) {
        std::cout << "No available moves to evaluate.\n";
        return {};
    }

    std::vector<double> S1(available_moves.size());
    std::vector<double> S2(available_moves.size());
    std::map<std::pair<size_t, size_t>, double> S1_diff{};
    std::map<std::pair<size_t, size_t>, double> S2_diff{};

    const auto opp_hand_copy { Cuarenta::opposing_player_state(game).hand.cards };

    int NUM_ITER { bot.num_mc_iters_ };

    for (int unused{}; unused < NUM_ITER; unused++) {

        for (auto& rank : Cuarenta::opposing_player_state(game).hand.cards) {
            rank = bot.weighted_random_rank();
        }

        std::vector<double> temp_moves_S1(available_moves.size());
        std::vector<double> temp_moves_S2(available_moves.size());
        for (size_t i{}; i < available_moves.size(); i++) {

            const Cuarenta::Undo undo { Cuarenta::make_move_in_place(game, Cuarenta::Move{available_moves.at(i)}) };
            game.advance_turn();
            double value { -minimax(game, depth - 1) };
            game.unadvance_turn();
            Cuarenta::undo_move_in_place(game, undo);

            S1[i] += value;
            S2[i] += value;
            temp_moves_S1[i] = value;
            temp_moves_S2[i] = value * value;
        }
        Cuarenta::opposing_player_state(game).hand.cards = opp_hand_copy;

        for (size_t i{}; i < available_moves.size(); i++) {
            for (size_t j{}; j < available_moves.size(); j++) {
                if (i >= j) { continue; }
                S1_diff[std::pair<size_t, size_t>{i, j}] += (temp_moves_S1[j] - temp_moves_S1[i]);
                S2_diff[std::pair<size_t, size_t>{i, j}] += ((temp_moves_S2[j] - temp_moves_S1[i]) * (temp_moves_S2[j] - temp_moves_S1[i]));
            }
        }
    }

    std::vector<MoveEval> move_evaluations;
    bool is_confident {true};
    MoveEval best_move { .eval = std::numeric_limits<double>::lowest(), 
                         .move = {},
                         .std_dev = {}}; 
    move_evaluations.reserve(available_moves.size());

    for (size_t i{}; i < available_moves.size(); i++) {
        const double mean { S1[i] / NUM_ITER };
        const double var  { S2[i] / NUM_ITER - mean * mean };
        move_evaluations.emplace_back(MoveEval{ 
            .move = Cuarenta::Move{available_moves.at(i)},
            .eval = mean,
            .std_dev = std::sqrt(std::max(0.0, var))
        });
        if (mean > best_move.eval) { best_move = move_evaluations.back(); }
    }

    assert(S1_diff.size() == (available_moves.size() * (available_moves.size() - 1) / 2));
    for (const auto& [idx, val] : S1_diff) {
        const double mean {S1_diff[idx] / NUM_ITER};
        const double var {S2_diff[idx] / NUM_ITER - mean * mean};
        const double stddev = std::sqrt(std::max(0.0, var));

        // 99.9%
        const double lower = mean - 3.29053 * stddev;
        const double upper = mean + 3.29053 * stddev;
        if(lower <= 0.0 && upper >= 0.0) { is_confident = false; }
    }

    return std::pair<MoveEval, bool>{best_move, is_confident};
}


}