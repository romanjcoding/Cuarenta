#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <bit>

namespace Cuarenta {

enum class RankMask : uint16_t {}; // may hold multiple bits
enum class Rank     : uint16_t { 
    Ace=1,
    Two=2, Three=4, Four=8, Five=16, Six=32, Seven=64, 
    Jack=128, Queen=256, King=512,
    Invalid = 0,
};

constexpr int MAX_MOVES_PER_BOARD {16};
constexpr int NUM_RANK_BITS {std::numeric_limits<std::underlying_type_t<RankMask>>::digits};

constexpr uint16_t to_u16(Rank r)      { return static_cast<uint16_t>(r); }
constexpr uint16_t to_u16(RankMask m)  { return static_cast<uint16_t>(m); }

constexpr RankMask to_mask(Rank r)     { return static_cast<RankMask>(r); }
constexpr RankMask to_mask(uint16_t t) { return static_cast<RankMask>(t); }

constexpr uint16_t ALL_RANK_BITS {
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
    if (is_valid_rank(rank)) { return rank; }
    else { 
        throw std::invalid_argument("invalid (non-singular bit pattern) casted to Rank.\n"); 
    }
}

constexpr Rank to_rank(RankMask r) {
    Rank rank { static_cast<Rank>(r) };
    if (is_valid_rank(rank)) { return rank; }
    else {
        throw std::invalid_argument("invalid (non-singular bit pattern) casted to Rank.\n");
    }
}


constexpr RankMask operator &(const RankMask& rank_a, const RankMask& rank_b) { 
    return static_cast<RankMask>(to_u16(rank_a) & to_u16(rank_b));
}

constexpr RankMask operator |(const RankMask& rank_a, const RankMask& rank_b) { 
    return static_cast<RankMask>(to_u16(rank_a) | to_u16(rank_b));
}

constexpr RankMask operator ~(const RankMask& rank) { 
    return static_cast<RankMask>(~to_u16(rank));
}

/* BOARD: A234567JQK
    10 possible non-additions (5 per hand.)
            A -> 0
            2 -> 0
            3: A+2 -> 1
            4: A+3 -> 1
            5: A+4, 2+3 -> 2
            6: A+5, 4+2, A+2+3-> 3
            7: A+6, 2+5, 3+4, A+2+4 -> 4
            Maximized by holding 76543 = 4+3+2+1+1+5 = 16
*/

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

constexpr int rank_to_int (Rank rank) {
    switch (rank) {
        case Rank::Ace:   return 1;
        case Rank::Two:   return 2;
        case Rank::Three: return 3;
        case Rank::Four:  return 4;
        case Rank::Five:  return 5;
        case Rank::Six:   return 6;
        case Rank::Seven: return 7;
        default:          return 0;
    }
}

constexpr Rank int_to_rank(int val) {
    if (val == 0) return Rank::Invalid;
    return to_rank(static_cast<uint16_t>(1 << (val - 1)));
}

constexpr Rank& operator++(Rank& rank, int) {
    switch(rank) {
        case Rank::Ace   : return rank = Rank::Two;
        case Rank::Two   : return rank = Rank::Three;
        case Rank::Three : return rank = Rank::Four;
        case Rank::Four  : return rank = Rank::Five;
        case Rank::Five  : return rank = Rank::Six;
        case Rank::Six   : return rank = Rank::Seven;
        case Rank::Seven : return rank = Rank::Jack;
        case Rank::Jack  : return rank = Rank::Queen;
        case Rank::Queen : return rank = Rank::King;
        case Rank::King  : return rank = Rank::Invalid;
        case Rank::Invalid:
            throw std::out_of_range("Cannot increment Rank::Invalid");
    }
}

constexpr bool operator<(Rank a, Rank b) {
    return rank_to_int(a) < rank_to_int(b);
}

bool inline contains_ranks(RankMask cards, RankMask ranks) {
    return (to_u16(cards) & to_u16(ranks));
}

struct Hand {

   std::vector<Rank> cards;

   void print_hand() { 
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

Deck make_cuarenta_deck();
Hand generate_hand(Deck& d);

bool is_valid_move(const Move& move, const Table& table);
int play_card(const Move& move, Hand& hand, Table& table);

void remove_ranks(RankMask& cards, const RankMask to_remove);
void sequence_waterfall(RankMask cards, const Card& start_card);

struct Game1v1 {
    Deck deck { make_cuarenta_deck() };
    Table table{};
};
}