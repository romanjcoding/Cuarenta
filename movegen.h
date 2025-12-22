#pragma once
#include "cuarenta.h"
#include <vector>

namespace Cuarenta {

struct MoveList {
    std::array<Move, MAX_MOVES_PER_TABLE> moves{};
    uint8_t n = 0;
};

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


MoveList generate_all_moves(const Game_State& game);
}