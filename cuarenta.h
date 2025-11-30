#pragma once
#include "rank.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <bit>

namespace Cuarenta {

struct Card {
    Rank rank_;
    bool operator==(const Card& card) const { return card.rank_ == rank_; }
};

struct Deck {
    std::vector<Rank> cards;
};

struct Move {
    // Move includes played-card and all captured cards
    RankMask table_targets{0};
};

bool inline contains_ranks(RankMask cards, RankMask ranks) {
    return (to_u16(cards) & to_u16(ranks));
}

struct Hand {

   std::vector<Rank> cards;

   void print_hand() { 
        std::cout << "Hand: ";
        for (const Rank& rank : cards) {
            std::cout << rank_to_str(rank);
        }
        std::cout << '\n';
    }
};

struct Table {

    RankMask cards;
    Rank last_played_card{ Rank::Invalid };

    void print_table() {
        std::cout << "TABLE: ";
        for (Rank rank = Rank::Ace; rank != Rank::Invalid; rank++) {
            if (contains_ranks(cards, to_mask(rank))) {
                std::cout << rank_to_str(rank);
            }
        }
        std::cout << '\n';
    }
};

struct Game_State {
    Hand hand_p1;
    Hand hand_p2;
    Table table;
    int p1_captured_cards_count;
    int p2_captured_cards_count;
    int p1_score;
    int p2_score;
};

Game_State play_card(const Game_State& game_state, const Move& move);


Deck make_cuarenta_deck();
Hand generate_hand(Deck& d);

bool is_valid_move(const Move& move, const Table& table);
int play_card(const Move& move, Hand& hand, Table& table);

void remove_ranks(RankMask& cards, const RankMask to_remove);
void sequence_waterfall(RankMask& cards, const Rank start_card);

struct Game1v1 {
    Deck deck { make_cuarenta_deck() };
    Table table{};
};
}