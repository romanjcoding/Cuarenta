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

constexpr int heuristic_value(const Cuarenta::Game_State& game_state) {
    return Cuarenta::current_player_state(game_state).score - 
           Cuarenta::opposing_player_state(game_state).score; // +____ ?
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

int minimax(Cuarenta::Game_State& game_state, const int depth) {

    if (Cuarenta::opposing_player_state(game_state).score >= 40) {
        return std::numeric_limits<int>::min();
    }

    if (Cuarenta::current_player_state(game_state).hand.cards.empty()) {
        return deterministic_value(game_state);
    }

    if (depth == 0) { return heuristic_value(game_state); }

    std::vector<Cuarenta::Move> avialable_moves { Cuarenta::generate_all_moves(game_state) };

    int value { std::numeric_limits<int>::min() };
    for (const Cuarenta::Move& move : avialable_moves) {
        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        updated_state.advance_turn();
        value = std::max(value, -minimax(updated_state, depth - 1));
    }
    return value;
}

std::vector<Move_Eval> evaluate_all_moves(
    const Cuarenta::Game_State& game_state, 
    const int depth) {

    std::vector<Cuarenta::Move> avialable_moves  { Cuarenta::generate_all_moves(game_state) };
    std::vector<Move_Eval>      move_evaluations {};
    move_evaluations.reserve(avialable_moves.size());

    if (avialable_moves.empty()) { 
        std::cout << "No available moves to evaluate.\n";
        return {}; 
    }

    for (const Cuarenta::Move& move : avialable_moves) {

        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        updated_state.advance_turn();

        int value = -minimax(updated_state, depth - 1);

        std::cout << "Analyzed move: "
          << std::left << std::setw(15) << Cuarenta::mask_to_str(move.targets_mask)
          << " eval = "
          << ((value > 0) ? "+" : "")
          << value << "\n";

        move_evaluations.push_back(Move_Eval{.move = move, .value = value});
    }
    return move_evaluations;
}

Move_Eval choose_best_move(
    const Cuarenta::Game_State& game_state, 
    const int depth) {

    std::vector<Cuarenta::Move> avialable_moves  { Cuarenta::generate_all_moves(game_state) };
    if (avialable_moves.empty()) { return {}; }

    int best_value { std::numeric_limits<int>::min() };
    auto best_move { Move_Eval{ .move = avialable_moves.front(),
                                .value = best_value } };

    for (const Cuarenta::Move& move : avialable_moves) {

        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        updated_state.advance_turn();

        int value = -minimax(updated_state, depth - 1);

        std::cout << "Analyzed move: "
          << std::left << std::setw(15) << Cuarenta::mask_to_str(move.targets_mask)
          << " eval = "
          << ((value > 0) ? "+" : "")
          << value << "\n";

        if (value > best_value) {
            best_move = Move_Eval{ .move = move, .value = value };
        }
    }
    return best_move;
}

}