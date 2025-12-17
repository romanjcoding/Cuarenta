#pragma once
#include "cuarenta.h"
#include <vector>

namespace Cuarenta {


constexpr std::initializer_list<RankMask> ADDITIONS_BY_RANK[] = {
    {},
    {},
    {},
    {   to_mask(Rank::Ace) | to_mask(Rank::Two)   },
    {   to_mask(Rank::Ace) | to_mask(Rank::Three) }, 
    {
        to_mask(Rank::Ace) | to_mask(Rank::Four),
        to_mask(Rank::Two) | to_mask(Rank::Three) }, 
    {
        to_mask(Rank::Ace)  | to_mask(Rank::Five),
        to_mask(Rank::Four) | to_mask(Rank::Two),
        to_mask(Rank::Ace)  | to_mask(Rank::Two) | to_mask(Rank::Three) }, 
    {
        to_mask(Rank::Ace)   | to_mask(Rank::Six),
        to_mask(Rank::Two)   | to_mask(Rank::Five),
        to_mask(Rank::Three) | to_mask(Rank::Four),
        to_mask(Rank::Ace)   | to_mask(Rank::Two) | to_mask(Rank::Four) },
    {},
    {},
    {}
};

constexpr RankMask LOW_MASK { to_mask(Rank::Ace)   | to_mask(Rank::Two)  |
                              to_mask(Rank::Three) | to_mask(Rank::Four) |
                              to_mask(Rank::Five)  | to_mask(Rank::Six) };

std::vector<Move> generate_all_moves(const Game_State& game_state);
}