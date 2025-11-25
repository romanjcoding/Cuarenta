#include "cuarenta.h"
#include <random>
#include <algorithm>
#include <optional>
#include <utility>
#include <cassert>
#include <limits>

namespace Cuarenta {

Deck make_cuarenta_deck() {

    Deck d;
    d.cards.reserve(40);

    static constexpr Rank ranks[] = {
        Rank::Ace,
        Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six, Rank::Seven,
        Rank::Jack, Rank::Queen, Rank::King
    };

    for (Rank r : ranks) {
        for (int i=0; i<4; i++) {
            d.cards.push_back(r);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(d.cards.begin(), d.cards.end(), g);
    return d;
}

bool is_valid_move(const Move& move, const Table& table) {
    return true;
}

Hand generate_hand(Deck& d) {
    if (d.cards.size() < 5) {
        throw std::runtime_error("Error: deck too small");
    }
    Hand h;
    h.cards.reserve(5);
    std::move(d.cards.begin(), d.cards.begin() + 5, std::back_inserter(h.cards));
    d.cards.erase(d.cards.begin(), d.cards.begin() + 5);
    return h;
}

void remove_ranks(RankMask& cards, const RankMask to_remove) {
    cards = cards & ~(cards & to_remove);
}

void add_ranks(RankMask& cards, const RankMask to_add) {
    cards = cards | to_add;
}

void sequence_waterfall(RankMask& cards, const Rank start_card) {
    Rank current_card = start_card;
    while (current_card != Rank::King) {
        current_card++;
        if (contains_ranks(cards, to_mask(current_card))) { 
            remove_ranks(cards, to_mask(current_card));
        }
        else { return; }
    }
}


int play_card(const Move& move, Hand& hand, Table& table) {

    assert(is_valid_move(move, table) && "play_card precondition violated");

    int maximum_played_bit { NUM_RANK_BITS - std::countl_zero(to_u16(move.table_targets)) };
    Rank played_card { int_to_rank(maximum_played_bit) };

    int points_scored { 0 };

    // Move is a simple capture or add-to-table, non-addition
    if (std::has_single_bit(to_u16(move.table_targets))) {

        if (contains_ranks(table.cards, move.table_targets)) {
            
            remove_ranks(table.cards, move.table_targets);

            // caida, +2pts
            if (played_card == table.last_played_card) { 
                points_scored += 2;
                table.last_played_card = Rank::Invalid; //NEED TO DO FOR ADDITION

            }
            sequence_waterfall(table.cards, played_card);
        }

        else {
            table.last_played_card = played_card;
            add_ranks(table.cards, move.table_targets);
        }
    }

    // Move is an addition
    else {
        if (contains_ranks(table.cards, move.table_targets)) {
            remove_ranks(table.cards, move.table_targets);
            sequence_waterfall(table.cards, played_card);
        }
        else { 
            throw std::invalid_argument("Invalid RankMask used, cards are not on the table.\n"); 
        }
    }

    // limpia, +2 points
    if (to_u16(table.cards) == 0) {
        points_scored += 2;
    }

    return points_scored;
}
}