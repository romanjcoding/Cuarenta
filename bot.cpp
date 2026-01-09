#include "bot.h"

#include "game_state.h"
#include "dynamic_array.h"
#include "cuarenta.h"
#include "movegen.h"

#include <limits>
#include <algorithm>
#include <bit>
#include <utility>
#include <random>
#include <iomanip>
#include <iterator>
#include <stdexcept>

namespace Bot {

constexpr double heuristic_value(const Cuarenta::Game_State& game_state) {
    return Cuarenta::current_player_state(game_state).score  - 
           Cuarenta::opposing_player_state(game_state).score;
}

constexpr int deterministic_value(const Cuarenta::Game_State& game_state) {

    int captured_cards_score { 0 };

    auto player { Cuarenta::current_player_state(game_state) };
    auto enemy  { Cuarenta::opposing_player_state(game_state) };

    // Scoring rules: 20 cards = 6pts, 22 cards = 8pts, etc.
    if (player.num_captured_cards >= 20) {
            captured_cards_score += 6 + 2 * ((player.num_captured_cards - 20) / 2);
    }

    if (enemy.num_captured_cards >= 20) {
            captured_cards_score -= 6 + 2 * ((enemy.num_captured_cards - 20) / 2);
    }

    return player.score - enemy.score + captured_cards_score;
}

std::pair<Cuarenta::Rank, size_t> monte_carlo_idx(std::map<Cuarenta::Rank, util::dynamic_array<double, 4>> hand_prob) {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    double tot_sum{};
    for (auto pair : hand_prob) {
        for (size_t i{}; i < pair.second.size(); i++) {
            tot_sum += pair.second.at(i);
            pair.second.at(i) = tot_sum;
        }
    }

    std::uniform_real_distribution<double> dis(0, tot_sum);
    double rand { dis(gen) };

    for (const auto pair : hand_prob) {
        for (size_t idx{}; idx < pair.second.size(); idx++) {
            if (rand >= pair.second.at(idx)) { return std::pair{pair.first, idx}; }
        }
    }
    throw std::out_of_range("Error with monte-carlo RNG generation");
}

double minimax(Cuarenta::Game_State& game_state, const int depth) {

    if (Cuarenta::opposing_player_state(game_state).score >= 40) {
        return std::numeric_limits<double>::lowest();
    }

    if (Cuarenta::current_player_state(game_state).hand.cards.empty()) {
        return static_cast<double>(deterministic_value(game_state));
    }

    if (depth == 0) { 
        return heuristic_value(game_state); 
    }

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

std::vector<Move_Eval> evaluate_all_moves_mc (
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

    for (int unused{}; unused< NUM_ITER; unused++) {
        static_cast<void>(unused);

        for (auto& rank : Cuarenta::opposing_player_state(game_state).hand.cards) {
            rank = monte_carlo_idx(bot.hand_prob).first;
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

    std::vector<Move_Eval> move_evaluations;
    move_evaluations.reserve(available_moves.size());

    for (size_t i{}; i < available_moves.size(); i++) {
        const double mean { S1[i] / NUM_ITER };
        const double var  { S2[i] / NUM_ITER - mean * mean };
        move_evaluations.emplace_back(Move_Eval{ 
            .move = Cuarenta::Move{available_moves.at(i)},
            .eval = mean,
            .std_dev = std::sqrt(var) // clamp to std::max(0.0, std::sqrt(var))?
        });
    }
    return move_evaluations;
}
}