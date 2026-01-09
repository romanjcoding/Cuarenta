#pragma once

#include "cuarenta.h"
#include "dynamic_array.h"

#include <vector>
#include <cassert>

namespace Cuarenta {

static constexpr std::initializer_list<RankMask> ADDITIONS_BY_RANK[] = {
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


util::dynamic_array<RankMask, MAX_MOVES_PER_TABLE> generate_all_moves(const Game_State& game);

}