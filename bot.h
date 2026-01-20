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

    // todo: add multipliers for caida/limpia?
    void update_from_move(Cuarenta::Move enemy_move) {
        const auto played_card { enemy_move.get_played_rank() };
        assert(hand_prob[played_card].count >= 1);
        hand_prob[played_card].count--;
    }

    void update_from_hand(const Cuarenta::Hand hand) {
        for (const auto& card : hand.cards) {
            update_from_move(Cuarenta::Move{to_mask(card)});
        }
    }
    void reset_probabilities() {
        for (auto& [rank, prob] : hand_prob) {
            prob = RankProbability{};
        }
    }

    Cuarenta::Rank weighted_random_rank() const {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        double tot_sum{};
        for (const auto& [rank, rank_prob] : hand_prob) {
            tot_sum += rank_prob.count * rank_prob.probability_weight;
        }

        std::uniform_real_distribution<double> dis(0, tot_sum);
        double rand { dis(gen) };

        for (const auto& [rank, rank_prob] : hand_prob) {
            rand -= rank_prob.count * rank_prob.probability_weight;
            if (rand <= 0) { return rank; }
        }
        throw std::out_of_range("Error with monte-carlo RNG generation");
    }
};

const static Bot BOT_CHILD { Bot{ 1 } };
const static Bot BOT_ROBOT { Bot{ 10 } };
const static Bot BOT_MAN   { Bot{ 1000 } };
const static Bot BOT_CHEAT { Bot{ 10000 } };

constexpr double heuristic_value(const Cuarenta::Game_State& game, int round);

double minimax(Cuarenta::Game_State& game, const int depth);

std::vector<MoveEval> evaluate_all_moves_mc(
    const Bot& bot,
    Cuarenta::Game_State& game, 
    const int depth);

constexpr int deck_size_to_round(size_t deck_size) {
    switch (deck_size) {
        case 30uz: return 1;
        case 20uz: return 2;
        case 10uz: return 3;
        case 0uz:  return 4;
    }
    throw std::runtime_error("Error with deck_size bro");
}

}