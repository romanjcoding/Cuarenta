#pragma once
#include "cuarenta.h"
#include "game_state.h"
#include "rank.h"
#include "ansi.h"
#include <vector>
#include <array>
#include <utility>
#include <mdspan>

namespace Bot {

enum class BotType : uint8_t { CHILD, ROBOT, OLDMAN, CHEAT };

struct Move_Eval {
    Cuarenta::Move move;
    double eval; // negamax value from the current player's perspective
    double std_dev;
};

struct Hand_Probability {
    std::array<double, Cuarenta::NUM_CARDS> p1;
};

constexpr Cuarenta::Rank idx_to_rank(size_t idx) {
    return Cuarenta::int_to_rank((static_cast<int>(idx) / Cuarenta::NUM_CARDS_PER_RANK + 1));
};

// returns idx at the beginning of the 4 ranks
constexpr size_t rank_to_idx(Cuarenta::Rank rank) {
    return static_cast<size_t>(
        (Cuarenta::rank_to_int(rank) - 1) * Cuarenta::NUM_CARDS_PER_RANK);
}

struct Bot {
    Hand_Probability hand_probabilities_;
    int num_mc_iters_;

    constexpr Bot(int num_mc_iters) : 
        hand_probabilities_{ std::array<double, Cuarenta::NUM_CARDS> {
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        } },
        num_mc_iters_{num_mc_iters} {}

    void update_probabilities(Cuarenta::Move enemy_move) {
        auto played_card { enemy_move.get_played_rank() };
        size_t idx       { rank_to_idx(played_card) };
        for (size_t i{}; i < Cuarenta::NUM_CARDS_PER_RANK; i++) {
            if (hand_probabilities_.p1[idx + i] > 0) {
                hand_probabilities_.p1[idx + i] = 0;
                break;
            }
        }
    }
    void initialize_probabilities(Cuarenta::Hand hand) {
        for (auto& card : hand.cards) {
            update_probabilities(Cuarenta::Move{to_mask(card)});
        }
    }
};

static constexpr Bot BOT_CHILD { Bot{1} };
static constexpr Bot BOT_ROBOT { Bot{10} };
static constexpr Bot BOT_CHEAT { Bot{10000} };


constexpr double heuristic_value(const Cuarenta::Game_State& game);
constexpr int deterministic_value(const Cuarenta::Game_State& game);

size_t monte_carlo_idx(Hand_Probability prob);

Cuarenta::Move make_move(const Cuarenta::Game_State& game, BotType bot);

double minimax(Cuarenta::Game_State& game, const int depth);

std::vector<Move_Eval> evaluate_all_moves_mc(
    const Bot& bot,
    Cuarenta::Game_State& game, 
    const int depth);

}