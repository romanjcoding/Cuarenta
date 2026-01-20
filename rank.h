#pragma once
#include <iostream>
#include <cstdint>
#include <limits>
#include <bit>
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>

namespace Cuarenta {

enum class RankMask : uint16_t {}; // may hold multiple bits as a mask
enum class Rank     : uint16_t {   // singular-bitted, invariant enforced by helper casts
    Ace=1,
    Two=2, Three=4, Four=8, Five=16, Six=32, Seven=64, 
    Jack=128, Queen=256, King=512, 
    Invalid = 0,
};

static constexpr size_t MAX_MOVES_PER_TABLE { 16 };
static constexpr size_t NUM_RANKS { 10 };
static constexpr size_t NUM_RANK_BITS { std::numeric_limits<std::underlying_type_t<RankMask>>::digits };

// helper casts between Rank, RankMask, and uint16_t
constexpr uint16_t to_u16(Rank r)      { return static_cast<uint16_t>(r); }
constexpr uint16_t to_u16(RankMask m)  { return static_cast<uint16_t>(m); }
constexpr RankMask to_mask(Rank r)     { return static_cast<RankMask>(r); }
constexpr RankMask to_mask(uint16_t t) { return static_cast<RankMask>(t); }

static constexpr uint16_t ALL_RANK_BITS {
    to_u16(Rank::Ace)   | to_u16(Rank::Two)   | to_u16(Rank::Three) |
    to_u16(Rank::Four)  | to_u16(Rank::Five)  | to_u16(Rank::Six)   |
    to_u16(Rank::Seven) | to_u16(Rank::Jack)  | to_u16(Rank::Queen) |
    to_u16(Rank::King)};

constexpr bool is_valid_rank(Rank r) {
    uint16_t m = to_u16(r);
    return (m == 0) ||
           (std::has_single_bit(m) && (m & ~ALL_RANK_BITS) == 0);
}

constexpr Rank to_rank(uint16_t r) {
    Rank rank { static_cast<Rank>(r) };
    assert(is_valid_rank(rank));
    return rank;
}
constexpr Rank to_rank(RankMask r) {
    Rank rank { static_cast<Rank>(r) };
    assert(is_valid_rank(rank));
    return rank;
}

bool inline contains_ranks(RankMask cards, RankMask ranks) {
    return (to_u16(cards) & to_u16(ranks)) == to_u16(ranks);
}

constexpr std::string rank_to_str (Rank rank) {
    switch (rank) {
        case Rank::Ace:   return "A";
        case Rank::Two:   return "2";
        case Rank::Three: return "3";
        case Rank::Four:  return "4";
        case Rank::Five:  return "5";
        case Rank::Six:   return "6";
        case Rank::Seven: return "7";
        case Rank::Jack:  return "J";
        case Rank::Queen: return "Q";
        case Rank::King:  return "K";
        default: break;
    }
    return "?";
}

constexpr std::string mask_to_str(RankMask mask) {
    std::string out;

    uint16_t bits = to_u16(mask) & ALL_RANK_BITS;
    if (bits == 0) {
        return out;
    }

    // Find the highest (top) bit
    Rank top_rank = Rank::Invalid;
    for (int i = NUM_RANK_BITS - 1; i >= 0; --i) {
        uint16_t bit = static_cast<uint16_t>(1u << i);
        if (bits & bit) {
            top_rank = to_rank(bit);
            bits &= static_cast<uint16_t>(~bit);
            break;
        }
    }

    if (!is_valid_rank(top_rank) || top_rank == Rank::Invalid) {
        return out;
    }

    out += rank_to_str(top_rank);

    if (bits == 0) {
        return out;
    }

    out += " = ";

    bool first = true;
    for (size_t i{}; i < NUM_RANK_BITS; i++) {
        uint16_t bit = static_cast<uint16_t>(1u << i);
        if (bits & bit) {
            if (!first) {
                out += "+";
            }
            out += rank_to_str(to_rank(bit));
            first = false;
        }
    }

    return out;
}

constexpr int rank_to_int (Rank rank) {
    return static_cast<int>(NUM_RANK_BITS) - std::countl_zero(to_u16(rank));
}
constexpr Rank int_to_rank(int val) {
    assert(val >= 0 && val < 11);
    return to_rank(static_cast<uint16_t>(1 << (val - 1)));
}

constexpr std::vector<Rank> mask_to_vector(RankMask mask) {
    std::vector<Rank> ret;
    ret.reserve(NUM_RANKS);
    for (size_t i{1}; i <= NUM_RANKS; i++) {
        auto rank { int_to_rank(static_cast<int>(i)) };
        if (contains_ranks(mask, to_mask(rank))) { ret.push_back(rank); }
    }
    return ret;
}

constexpr RankMask operator &(const RankMask& rank_a, const RankMask& rank_b) { 
    return static_cast<RankMask>(to_u16(rank_a) & to_u16(rank_b));
}
constexpr RankMask operator |(const RankMask& rank_a, const RankMask& rank_b) { 
    return static_cast<RankMask>(to_u16(rank_a) | to_u16(rank_b));
}
constexpr RankMask operator ~(const RankMask& rank) { 
    return static_cast<RankMask>(~to_u16(rank) & ALL_RANK_BITS);
}
constexpr Rank operator++(Rank& rank, int) {
    assert(rank != Rank::King);
    Rank old = rank;
    rank = to_rank(static_cast<uint16_t>(to_u16(rank) << 1));
    return old;
}

constexpr Rank operator--(Rank& rank, int) {
    assert(rank != Rank::Ace);
    Rank old = rank;
    rank = to_rank(static_cast<uint16_t>(to_u16(rank) >> 1));
    return old;
}

constexpr Rank prev_rank(Rank rank) {
    assert(rank != Rank::Ace);
    return to_rank(static_cast<uint16_t>(to_u16(rank) >> 1));
}

constexpr bool operator <(Rank a, Rank b) {
    return rank_to_int(a) < rank_to_int(b);
}

constexpr bool operator >(Rank a, Rank b) {
    return rank_to_int(a) > rank_to_int(b);
}

} // namespace Rank