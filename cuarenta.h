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

Deck make_cuarenta_deck();
Hand generate_hand(Deck& d);

bool is_valid_move(const Move& move, const Table& table);
Game_State make_move(Game_State game_state, const Move& move);
void update_captured_cards(Game_State& game_state);

void remove_ranks(RankMask& cards, const RankMask to_remove);
int sequence_waterfall (RankMask& cards, const Rank start_card);

void print_move(const Game_State& game_state, const Move& move);

}