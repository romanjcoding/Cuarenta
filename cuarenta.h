#pragma once
#include "rank.h"
#include "game_state.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <bit>

namespace Cuarenta {

static constexpr int NUM_CARDS {40};

struct Card {
    Rank rank_;
    bool operator==(const Card& card) const { return card.rank_ == rank_; }
};

struct Deck {
    std::vector<Rank> cards;
};

// Invariant: in table_targets, the MSB is the played card rank.
// Any lower bits (if present) are addition sums.
struct Move {
    RankMask targets_mask{0};
    Rank get_played_rank() const {
        const int largest_bit  { NUM_RANK_BITS - std::countl_zero(to_u16(targets_mask)) };
        return int_to_rank(largest_bit);
    }
};

Deck make_cuarenta_deck();
Hand generate_hand(Deck& d);

bool is_valid_move(const Move& move, const Table& table);
Game_State make_move(Game_State game_state, const Move& move);
void update_captured_cards(Game_State& game_state);

void remove_ranks(RankMask& cards, const RankMask to_remove);
int sequence_waterfall (RankMask& cards, const Rank start_card);

void print_move(const Game_State& game_state, const Move& move);

}