#pragma once
#include "cuarenta.h"
#include "dynamic_array.h"
#include "game_state.h"
#include "rank.h"
#include "ansi.h"
#include <vector>
#include <array>
#include <utility>
#include <map>

namespace Bot {

struct MoveEval {
    Cuarenta::Move move;
    double eval; // negamax value from the current player's perspective
    double std_dev;
};

struct RankProbability {
    double probability_weight { 1.0 };
    int count { 4 };
};

struct Bot {
    std::map<Cuarenta::Rank, RankProbability> hand_prob {
        { Cuarenta::Rank::Ace,   RankProbability{} },
        { Cuarenta::Rank::Two,   RankProbability{} },
        { Cuarenta::Rank::Three, RankProbability{} },
        { Cuarenta::Rank::Four,  RankProbability{} },
        { Cuarenta::Rank::Five,  RankProbability{} },
        { Cuarenta::Rank::Six,   RankProbability{} },
        { Cuarenta::Rank::Seven, RankProbability{} },
        { Cuarenta::Rank::Jack,  RankProbability{} },
        { Cuarenta::Rank::Queen, RankProbability{} },
        { Cuarenta::Rank::King,  RankProbability{} },
    };
    int num_mc_iters_;

    Bot(int num_mc_iters) : 
        num_mc_iters_{num_mc_iters} {}

    void update_from_move(Cuarenta::Move enemy_move) {
        const auto played_card { enemy_move.get_played_rank() };
        assert(hand_prob[played_card].count >= 1);
        hand_prob[played_card].count--;
    }

    void update_from_hand(Cuarenta::Hand hand) {
        for (const auto& card : hand.cards) {
            update_from_move(Cuarenta::Move{to_mask(card)});
        }
    }
    void reset_probabilities() {
        for (auto& [rank, prob] : hand_prob) {
            prob = RankProbability{};
        }
    }
};

const static Bot BOT_CHILD { Bot{ 1 } };
const static Bot BOT_ROBOT { Bot{ 10 } };
const static Bot BOT_MAN   { Bot{ 1000 } };
const static Bot BOT_CHEAT { Bot{ 10000 } };

constexpr double heuristic_value(const Cuarenta::Game_State& game);
constexpr int deterministic_value(const Cuarenta::Game_State& game);

std::pair<Cuarenta::Rank, size_t> monte_carlo_idx(std::map<Cuarenta::Rank, util::dynamic_array<float, 4>> arr);

double minimax(Cuarenta::Game_State& game, const int depth);

std::vector<MoveEval> evaluate_all_moves_mc(
    const Bot& bot,
    Cuarenta::Game_State& game, 
    const int depth);
}