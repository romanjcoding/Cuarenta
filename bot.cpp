#include "bot.h"
#include "game_state.h"
#include "cuarenta.h"
#include "movegen.h"
#include <limits>
#include <algorithm>
#include <bit>
#include <utility>
#include <random>
#include <iomanip>

namespace Bot {

enum class BotType : uint8_t { CHILD, ROBOT, OLDMAN, CHEAT };

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

Cuarenta::Move make_move(const Cuarenta::Game_State& game_state, BotType bot) {
    switch (bot) {
        case BotType::CHILD:
            return choose_best_move(game_state, 1).second; // low montecarlo
        case BotType::ROBOT:
            return choose_best_move(game_state, 2).second;
        case BotType::OLDMAN:
            return choose_best_move(game_state, 4).second;
        case BotType::CHEAT:
            return choose_best_move(game_state, 10).second;
    }
}

size_t monte_carlo_idx(std::array<double, Cuarenta::NUM_CARDS> prob) {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (size_t idx = 1; idx < prob.size(); idx++) {
        prob[idx] += prob[idx - 1];
    }
    
    double tot_sum { prob[prob.size() - 1] };
    std::uniform_real_distribution<> dis(0, tot_sum);
    double rand { dis(gen) };

    auto it      { std::ranges::lower_bound(prob, rand) };    
    size_t index { static_cast<size_t>(std::distance(prob.begin(), it)) };
    return index;
}

int minimax(Cuarenta::Game_State& game_state, const int depth) {

    if (Cuarenta::opposing_player_state(game_state).score >= 40) {
        return std::numeric_limits<int>::min();
    }

    // [ cards exhausted -> redraw hand? ]
    if (Cuarenta::current_player_state(game_state).hand.cards.empty()) {
        return deterministic_value(game_state);
    }

    if (depth == 0) { return heuristic_value(game_state); }

    std::vector<Cuarenta::Move> possible_moves { Cuarenta::generate_all_moves(
            game_state.table, 
            Cuarenta::current_player_state(game_state).hand
        ) };

    int value { std::numeric_limits<int>::min() };
    for (const Cuarenta::Move& move : possible_moves) {

        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        updated_state.advance_turn();
        value = std::max(value, -minimax(updated_state, depth - 1));
    }
    return value;
}

// TODO: Change into legible, not .first and .second
std::pair<int, Cuarenta::Move>
choose_best_move(const Cuarenta::Game_State& game_state, 
                const int depth) {

    std::vector<Cuarenta::Move> possible_moves { Cuarenta::generate_all_moves(
            game_state.table, 
            Cuarenta::current_player_state(game_state).hand
        ) };

    if (possible_moves.empty()) {
        return { heuristic_value(game_state), Cuarenta::Move{} };
    }

    int best_value = std::numeric_limits<int>::min();
    Cuarenta::Move best_move = possible_moves.front();

    for (const Cuarenta::Move& move : possible_moves) {

        Cuarenta::Game_State updated_state = Cuarenta::make_move(game_state, move);
        updated_state.advance_turn();

        int value = -minimax(updated_state, depth - 1);

        std::cout << "Analyzed move: "
          << std::left << std::setw(15) << Cuarenta::mask_to_str(move.targets_mask)
          << " eval = "
          << ((value > 0) ? "+" : "")
          << value << "\n";

        if (value > best_value) {
            best_value = value;
            best_move = move;
        }
    }

    return { best_value, best_move };
}

}