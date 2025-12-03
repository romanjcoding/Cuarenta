#pragma once
#include "rank.h"
#include "cuarenta.h"
#include <vector>
#include <iostream>

namespace Cuarenta {

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

enum class Player : uint8_t { P1 = 0, P2 = 1, NUM_PLAYERS };

struct Player_State {
    Hand hand{};
    int num_captured_cards{};
    int score{};
};

constexpr size_t to_index(const Player player) { return static_cast<size_t>(player); }

struct Game_State {
    Table table{};
    std::array<Player_State, to_index(Player::NUM_PLAYERS)> players {{
        { Hand{}, 0, 0 },
        { Hand{}, 0, 0 }
    }};
    Player to_move { Player::P1 };

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