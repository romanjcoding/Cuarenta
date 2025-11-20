#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

namespace Cuarenta {

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
constexpr int MAX_MOVES_PER_BOARD {16};

enum class Rank {
    Ace,
    Two, Three, Four, Five, Six, Seven,
    Jack, Queen, King,
    Num_Ranks
};

struct Card {
    Rank rank_;
    bool operator==(const Card& card) const { return card.rank_ == rank_; }
};

struct Deck {
    std::vector<Card> cards;
};

struct Move {
    Card card;
    int capture_offset{0};
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

constexpr Rank int_to_rank (int val) {
    switch (val) {
        case 1: return Rank::Ace;
        case 2: return Rank::Two;
        case 3: return Rank::Three;
        case 4: return Rank::Four;
        case 5: return Rank::Five;
        case 6: return Rank::Six;
        case 7: return Rank::Seven;
        default: break;
    }
    throw std::out_of_range("Bad input, can only conver 2,3,4,5,6,7 into rank");
}

inline Rank& operator++(Rank& rank, int) {
    switch(rank) {
        case Rank::Ace :   return rank = Rank::Two;
        case Rank::Two :   return rank = Rank::Three;
        case Rank::Three : return rank = Rank::Four;
        case Rank::Four :  return rank = Rank::Five;
        case Rank::Five :  return rank = Rank::Six;
        case Rank::Six :   return rank = Rank::Seven;
        case Rank::Seven : return rank = Rank::Jack;
        case Rank::Jack :  return rank = Rank::Queen;
        case Rank::Queen : return rank = Rank::King;
        case Rank::King:
            throw std::out_of_range("Cannot increment Rank::King");
        case Rank::Num_Ranks:
            throw std::out_of_range("Cannot increment Rank::Num_Ranks");
    }
}

struct Hand {

   std::vector<Card> cards;

   void print_hand() { 
        for (const Card& card : cards) {
            std::cout << rank_to_str(card.rank_);
        }
        std::cout << '\n';
    }
};

struct Table {

    std::vector<Card> cards;
    Card last_played_card{};

    Table() { cards.reserve(8); }
    void print_table() {
        std::cout << "TABLE: ";
        for (Card card : cards) {
            std::cout << rank_to_str(card.rank_);
        }
        std::cout << '\n';
    }
};

Deck make_cuarenta_deck();
Hand generate_hand(Deck& d);
bool check_and_remove(std::vector<Card>& cards, const Card& card);
int play_card(const Card& c, Table& t, int capture_offset = 0);
bool remove_rank_if_present(std::vector<Card>& cards, const Card& card);
bool contains_multiple(const std::vector<Card>& cards_A, const std::vector<Card>& cards_B);

struct Game1v1 {
    Deck deck { make_cuarenta_deck() };
    Table table{};
};
}