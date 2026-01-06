#pragma once

#include "cuarenta.h"

#include <vector>
#include <cassert>

namespace Cuarenta {

struct MoveList {
    std::array<Move, MAX_MOVES_PER_TABLE> moves{};
    size_t size = 0;

    static constexpr size_t capacity() { return MAX_MOVES_PER_TABLE; }
    bool full()  const { return size == capacity(); }
    bool empty() const { return size == 0; }

    bool contains(const Move& m) const {
        for (size_t i{}; i < size; i++) {
            if (m.targets_mask == moves[i].targets_mask) { return true; }
        }
        return false;
    }

    void push_back(const Move& m) {
        assert(size < capacity());
        moves[size++] = m;
    }
};

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


MoveList generate_all_moves(const Game_State& game);
}