#include "cuarenta.h"

#include <random>
#include <algorithm>
#include <optional>
#include <utility>
#include <cassert>
#include <limits>

namespace Cuarenta {

void remove_ranks(RankMask& cards, const RankMask to_remove) {
    cards = cards & ~(cards & to_remove);
}

bool is_card_in_hand(const Hand& hand, const Rank card) {
    auto iter { std::ranges::find(hand.cards, card) };
    return iter != hand.cards.end();
}

void remove_card_from_hand(Hand& hand, const Rank& card) {
    auto iter { std::ranges::find(hand.cards, card) };
    if (iter != hand.cards.end()) { hand.cards.erase(iter); }
}

void add_ranks(RankMask& cards, const RankMask to_add) {
    cards = cards | to_add;
}

int sequence_waterfall (RankMask& cards, const Rank start_card) {
    int num_cards_sequenced { 0 };
    Rank current_card = start_card;
    while (current_card != Rank::King) {
        current_card++;
        if (contains_ranks(cards, to_mask(current_card))) { 
            remove_ranks(cards, to_mask(current_card));
            num_cards_sequenced++;
        }
        else { return num_cards_sequenced; }
    }
    return num_cards_sequenced;
}

void update_captured_cards(Game_State& game) {
    for (Player_State& player_state : game.players) {
        // Scoring rules: 20 cards = 6pts, 22 cards = 8pts, etc.
        if (player_state.num_captured_cards >= 20) {
            player_state.score += 6 + 2 * ((player_state.num_captured_cards - 20) / 2);
        }
        player_state.num_captured_cards = 0;
    }
}

Undo make_move_in_place(Game_State& game, const Move& move) {

    Player_State& player_state { current_player_state(game) };
    Table& table               { game.table };
    
    const RankMask targets_mask { move.targets_mask };
    const bool is_addition      { !std::has_single_bit(to_u16(targets_mask)) };
    const Rank played_card      { move.get_played_rank() };

    Undo undo { .move = move,
                .last_played_card = table.last_played_card,
                .num_waterfalled_cards = 0 };

    const RankMask addition_mask { move.targets_mask & ~to_mask(played_card) };
    const bool table_has_capture { (is_addition)   ? 
        contains_ranks(table.cards, addition_mask) :
        contains_ranks(table.cards, to_mask(played_card)) };

    if (!is_card_in_hand(player_state.hand, played_card)) {
        player_state.hand.print_hand();
        throw std::invalid_argument(
            "Invalid RankMask used, played card (MSB in RankMask) is not in current players hand.\n"
        );
    }

    if (is_addition) {
        if (!table_has_capture) {
            throw std::invalid_argument(
            "Invalid RankMask used, cards are not on the table.\n"
            );
        }
        else {
            const int num_waterfalled_cards { 
                sequence_waterfall(table.cards, played_card) };
            const int num_cards_captured { 
                std::popcount(to_u16(targets_mask)) + num_waterfalled_cards };
            
            player_state.num_captured_cards += num_cards_captured;
            undo.num_waterfalled_cards += num_waterfalled_cards;
                
            remove_ranks(table.cards, targets_mask & ~to_mask(played_card));
            table.last_played_card = Rank::Invalid;
        }
    }

    else {
        if (!table_has_capture) {
            table.last_played_card = played_card;
            add_ranks(table.cards, targets_mask);
        }
        else {
            // Caída: +2 points
            if (played_card == table.last_played_card) {
                player_state.score += 2;
            }
            const int num_waterfalled_cards { 
                sequence_waterfall(table.cards, played_card) };
            const int num_cards_captured { 
                2 + num_waterfalled_cards };

            player_state.num_captured_cards += num_cards_captured;
            undo.num_waterfalled_cards += num_waterfalled_cards;

            remove_ranks(table.cards, to_mask(played_card));
            table.last_played_card = Rank::Invalid;
        }
    }

    // Limpia: +2 points
    if (to_u16(table.cards) == 0) {
        player_state.score += 2;
    }

    remove_card_from_hand(player_state.hand, played_card);
    return undo;
}

// pre-condition: Player to_move is the person that did make_move()
void undo_move_in_place(Game_State& game, const Undo& undo) {

    Player_State& player_state { current_player_state(game) };
    Table& table               { game.table };
    const Move& move           { undo.move };
    
    const RankMask targets_mask { move.targets_mask };
    const bool is_addition      { !std::has_single_bit(to_u16(targets_mask)) };
    Rank played_card            { move.get_played_rank() };

    const RankMask addition_mask { move.targets_mask & ~to_mask(played_card) };
    const bool card_on_table     { contains_ranks(table.cards, to_mask(played_card)) };

    player_state.hand.cards.push_back(played_card);
    table.last_played_card = undo.last_played_card;

    if ((played_card == undo.last_played_card) && !is_addition) { player_state.score -= 2; } // Caida
    if (to_u16(table.cards) == 0)                               { player_state.score -= 2; } // Limpia

    if (is_addition) { add_ranks(table.cards, addition_mask); }
    else {
        if (card_on_table) { remove_ranks(table.cards, to_mask(played_card)); }
        else               { add_ranks(table.cards, to_mask(played_card)); }
    }
    
    if (!card_on_table || is_addition) {
        int num_captured { (is_addition) ? std::popcount(to_u16(targets_mask)) : 2 };
        player_state.num_captured_cards -= num_captured + undo.num_waterfalled_cards;
    }

    // keep last, as it modifies played_card
    for (int i{}; i < undo.num_waterfalled_cards; i++) {
        played_card++;
        add_ranks(table.cards, to_mask(played_card));
    }
}

} // namespace Cuarenta