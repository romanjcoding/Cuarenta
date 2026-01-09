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

struct Move_Eval {
    Cuarenta::Move move;
    double eval; // negamax value from the current player's perspective
    double std_dev;
};

struct Bot {
    int num_mc_iters_;
    std::map<Cuarenta::Rank, util::dynamic_array<double, 4>> hand_prob = std::map { 
        std::pair { Cuarenta::Rank::Ace, util::dynamic_array<double,   4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Two, util::dynamic_array<double,   4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Three, util::dynamic_array<double, 4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Four, util::dynamic_array<double,  4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Five, util::dynamic_array<double,  4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Six, util::dynamic_array<double,   4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Seven, util::dynamic_array<double, 4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Jack, util::dynamic_array<double,  4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::Queen, util::dynamic_array<double, 4>{1.0, 1.0, 1.0, 1.0} },
        std::pair { Cuarenta::Rank::King, util::dynamic_array<double,  4>{1.0, 1.0, 1.0, 1.0} }
    };

    Bot(int num_mc_iters) : 
        num_mc_iters_{num_mc_iters} {}

    void update_probabilities(Cuarenta::Move enemy_move) {
        auto played_card { enemy_move.get_played_rank() };
        for (size_t i{}; i < hand_prob[played_card].size(); i++) {

        }
        
        }

    void initialize_probabilities(Cuarenta::Hand hand) {
        for (auto& card : hand.cards) {
            update_probabilities(Cuarenta::Move{to_mask(card)});
        }
    }
};

const static Bot BOT_CHILD { Bot{ 1 } };
const static Bot BOT_ROBOT { Bot{ 10 } };
const static Bot BOT_MAN   { Bot{ 100000 } };
const static Bot BOT_CHEAT { Bot{ 10000 } };

constexpr double heuristic_value(const Cuarenta::Game_State& game);
constexpr int deterministic_value(const Cuarenta::Game_State& game);

std::pair<Cuarenta::Rank, size_t> monte_carlo_idx(std::map<Cuarenta::Rank, util::dynamic_array<float, 4>> arr);

double minimax(Cuarenta::Game_State& game, const int depth);

std::vector<Move_Eval> evaluate_all_moves_mc(
    const Bot& bot,
    Cuarenta::Game_State& game, 
    const int depth);
}