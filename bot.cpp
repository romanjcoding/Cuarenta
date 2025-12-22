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

float captured_cards_to_pts (int num_captured_cards) {
    switch (num_captured_cards) {
        case 1 : return 0.25;
        case 2 : return 0.5;
        case 3 : return 0.75; 
        case 4 : return 1;
        case 5 : 
        default : return 0;
    }
}

constexpr float heuristic_value(const Cuarenta::Game_State& game_state) {
    return Cuarenta::current_player_state(game_state).score  - 
           Cuarenta::opposing_player_state(game_state).score;
}

constexpr int deterministic_value(const Cuarenta::Game_State& game_state) {

    int captured_cards_score { 0 };

    for (const Cuarenta::Player_State& player_state : game_state.players) {
        // Scoring rules: 20 cards = 6pts, 22 cards = 8pts, etc.
        if (player_state.num_captured_cards >= 20) {
            captured_cards_score = 6 + (player_state.num_captured_cards - 20) / 2;
        }
    }
    return Cuarenta::current_player_state(game_state).score - 
           Cuarenta::opposing_player_state(game_state).score + 
           captured_cards_score;
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

float minimax(Cuarenta::Game_State& game_state, const int depth) {

    if (Cuarenta::opposing_player_state(game_state).score >= 40) {
        return std::numeric_limits<float>::lowest();
    }

    if (Cuarenta::current_player_state(game_state).hand.cards.empty()) {
        return static_cast<float>(deterministic_value(game_state));
    }

    if (depth == 0) { 
        return heuristic_value(game_state); 
    }

    auto available_moves { Cuarenta::generate_all_moves(game_state) };

    float value { std::numeric_limits<float>::lowest() };
    for (size_t i{}; i < available_moves.n; i++) {

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

    auto available_moves { Cuarenta::generate_all_moves(game_state) };

    if (available_moves.n == 0) {
        std::cout << "No available moves to evaluate.\n";
        return {}; 
    }
    std::vector<float> S1(available_moves.n);
    std::vector<float> S2(available_moves.n);

    const auto opp_hand_copy { Cuarenta::opposing_player_state(game_state).hand.cards };

    int NUM_ITER { bot.num_mc_iters_ };

    for (int _{}; _<NUM_ITER; _++) {

        for (auto& rank : Cuarenta::opposing_player_state(game_state).hand.cards) {
            rank = idx_to_rank(monte_carlo_idx(bot.hand_probabilities_));
        }

        for (size_t i{}; i < available_moves.n; i++) {

            Cuarenta::Undo undo { Cuarenta::make_move_in_place(game_state, available_moves.moves[i]) };
            game_state.advance_turn();
        
            float value = -minimax(game_state, depth - 1);

            game_state.unadvance_turn();
            Cuarenta::undo_move_in_place(game_state, undo);

            S1[i] += value;
            S2[i] += value * value;
        }
        Cuarenta::opposing_player_state(game_state).hand.cards = opp_hand_copy;
    }
    
    std::vector<Move_Eval> move_evaluations(available_moves.n);
    for (size_t i{}; i < move_evaluations.size(); i++) {
        move_evaluations[i] = Move_Eval { 
            .move = available_moves.moves[i],
            .value = S1[i] / NUM_ITER,
            .std_dev = std::sqrt(
                    S2[i] / NUM_ITER - (S1[i] / NUM_ITER) * (S1[i] / NUM_ITER))
        };
    }
    return move_evaluations;
}
}