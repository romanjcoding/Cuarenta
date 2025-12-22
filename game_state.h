#pragma once
#include "rank.h"

#include <vector>
#include <iostream>
#include <random>
#include <algorithm>

#include "cuarenta.h"

namespace Cuarenta {

struct Card {
    Rank rank_;
    bool operator==(const Card& card) const { return card.rank_ == rank_; }
};

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

struct Deck {
    std::vector<Rank> cards;

    Deck(bool shuffled = false) {
        cards.reserve(40);
        static constexpr Rank ranks[] = {
            Rank::Ace,
            Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six, Rank::Seven,
            Rank::Jack, Rank::Queen, Rank::King
        };
        for (Rank r : ranks) {
            for (int i=0; i<4; i++) {
                cards.push_back(r);
            }
        }
        if (shuffled) {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(cards.begin(), cards.end(), g);
        }
    }

    Hand draw_hand() {
        if (cards.size() < 5) {
            throw std::runtime_error("Error: deck too small");
        }
        Hand h{};
        h.cards.reserve(5);
        std::move(cards.begin(), cards.begin() + 5, std::back_inserter(h.cards));
        cards.erase(cards.begin(), cards.begin() + 5);
        return h;
    }
};

// Invariant: in table_targets, the MSB is the played card rank.
// Any lower bits (if present) are addition sums.
struct Move {
    RankMask targets_mask{};
    Rank get_played_rank() const {
        const int largest_bit  { NUM_RANK_BITS - std::countl_zero(to_u16(targets_mask)) };
        return int_to_rank(largest_bit);
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

enum class Player : uint8_t { P1 = 0, P2 = 1, NUM_PLAYERS };

struct Player_State {
    Hand hand{};
    int num_captured_cards{};
    int score{};
};

constexpr size_t to_index(const Player player) { return static_cast<size_t>(player); }

struct Game_State {
    Table table{};
    Deck deck{};
    std::array<Player_State, to_index(Player::NUM_PLAYERS)> players {{
        { Hand{}, 0, 0 },
        { Hand{}, 0, 0 }
    }};
    Player to_move { Player::P1 };

    Game_State() {}

    Game_State(const Hand& hand1, const Hand& hand2) 
        : table{},
          players{{
              Player_State{ hand1, 0, 0 },
              Player_State{ hand2, 0, 0 }
          }},
          to_move{ Player::P1 } {}

    void advance_turn() {
        switch (to_move) {
            case Player::P1:
                to_move = Player::P2;
                break;
            case Player::P2:
                to_move = Player::P1;
                break;
            default:
                break;
        }
    }
    void unadvance_turn() {
        switch (to_move) {
            case Player::P1:
                to_move = Player::P2;
                break;
            case Player::P2:
                to_move = Player::P1;
                break;
            default:
                break;
        }
    }
};

constexpr const Player_State& state_for(const Game_State& game, const Player player) {
    return game.players[to_index(player)];
}

constexpr Player_State& state_for(Game_State& game, const Player player) {
    return game.players[to_index(player)];
}

constexpr const Player_State& current_player_state(const Game_State& g) {
    return state_for(g, g.to_move);
}

constexpr Player_State& current_player_state(Game_State& g) {
    return state_for(g, g.to_move);
}

constexpr Player_State& opposing_player_state(Game_State& g) {
    return g.players[1 - to_index(g.to_move)];
}

constexpr const Player_State& opposing_player_state(const Game_State& g) {
    return g.players[1 - to_index(g.to_move)];
}

}