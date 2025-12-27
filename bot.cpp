#include "bot.h"

#include <limits>
#include <algorithm>
#include <bit>
#include <utility>
#include <random>
#include <iomanip>

#include "game_state.h"
#include "cuarenta.h"
#include "movegen.h"

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

size_t monte_carlo_idx(Hand_Probability prob) {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (size_t idx{1uz}; idx < prob.p1.size(); idx++) {
        prob.p1[idx] += prob.p1[idx - 1];
    }
    
    double tot_sum { prob.p1[prob.p1.size() - 1] };
    std::uniform_real_distribution<> dis(0, tot_sum);
    double rand { dis(gen) };

    auto it      { std::ranges::lower_bound(prob.p1, rand) };    
    size_t index { static_cast<size_t>(std::distance(prob.p1.begin(), it)) };
    return index;
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
    for (size_t i{}; i < available_moves.size; i++) {

        Cuarenta::Undo undo { Cuarenta::make_move_in_place(game_state, available_moves.moves[i]) };

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

    if (available_moves.size == 0) {
        std::cout << "No available moves to evaluate.\n";
        return {}; 
    }
    std::vector<double> S1(available_moves.size);
    std::vector<double> S2(available_moves.size);

    const auto opp_hand_copy { Cuarenta::opposing_player_state(game_state).hand.cards };

    int NUM_ITER { bot.num_mc_iters_ };

    for (int _{}; _< NUM_ITER; _++) {

        for (auto& rank : Cuarenta::opposing_player_state(game_state).hand.cards) {
            rank = idx_to_rank(monte_carlo_idx(bot.hand_probabilities_));
        }

        for (size_t i{}; i < available_moves.size; i++) {

            const Cuarenta::Undo undo { Cuarenta::make_move_in_place(game_state, available_moves.moves[i]) };

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
    move_evaluations.reserve(available_moves.size);

    for (size_t i{}; i < available_moves.size; i++) {
        const double mean { S1[i] / NUM_ITER };
        const double var  { S2[i] / NUM_ITER - mean * mean };
        move_evaluations.emplace_back(Move_Eval{ 
            .move = available_moves.moves[i],
            .value = mean,
            .std_dev = std::sqrt(var) // clamp to std::max(0.0, std::sqrt(var))?
        });
    }
    return move_evaluations;
}
}