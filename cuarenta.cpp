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

// TODO : Fix Last_Played_card
Game_State make_move(Game_State game_state, const Move& move) {

    Player_State& player_state { current_player_state(game_state) };
    Table& table               { game_state.table };
    
    const RankMask targets_mask  { move.targets_mask };
    const bool is_single_capture { std::has_single_bit(to_u16(targets_mask)) };
    const bool targets_exist     { contains_ranks(table.cards, targets_mask) };

    const int largest_bit  { NUM_RANK_BITS - std::countl_zero(to_u16(targets_mask)) };
    const Rank played_card { int_to_rank(largest_bit) };

    // Non-single "addition" move must actually capture something
    if (!is_single_capture && !targets_exist) {
        throw std::invalid_argument(
            "Invalid RankMask used, cards are not on the table.\n"
        );
    }

    if (!is_card_in_hand(player_state.hand, played_card)) {
        throw std::invalid_argument(
            "Invalid RankMask used, played card (MSB in RankMask) is not in current players hand.\n"
        );
    }

    // TODO: Check if sums to correct value.

    if (!targets_exist) {
        table.last_played_card = played_card;
        add_ranks(table.cards, targets_mask);
    }

    else {

        // Caída: +2 points
        if (is_single_capture && played_card == table.last_played_card) {
            player_state.score += 2;
        }

        const int num_waterfalled_cards { 
            sequence_waterfall(table.cards, played_card) };
        
        const int num_captured_cards { is_single_capture ? 
            1 : std::popcount(to_u16(targets_mask)) + num_waterfalled_cards };

        player_state.captured_cards += num_captured_cards;
        
        remove_ranks(table.cards, targets_mask);
        table.last_played_card = Rank::Invalid;
    }

    // Limpia: +2 points
    if (to_u16(table.cards) == 0) {
        player_state.score += 2;
    }

    remove_card_from_hand(player_state.hand, played_card);
    return game_state;
}

void print_move(const Game_State& game_state, const Move& move) {

    // Who's playing?
    const Player current_player  { game_state.to_move };
    const std::size_t player_idx { to_index(current_player) };
    const Player_State& pstate   { game_state.players[player_idx] };

    // Played card + targets
    const int largest_bit  { NUM_RANK_BITS - std::countl_zero(to_u16(move.targets_mask)) };
    const Rank played_card { int_to_rank(largest_bit) };
    const auto played_bits { std::bitset<16>(to_u16(played_card)) };
    const auto target_bits { std::bitset<16>(to_u16(move.targets_mask)) };

    const bool is_single_capture { std::has_single_bit(to_u16(move.targets_mask)) };
    const bool targets_exist     { contains_ranks(game_state.table.cards, move.targets_mask) };

    std::cout << "========================= MOVE =========================\n";

    // Player info
    std::cout << "Player: "
              << (current_player == Player::P1 ? "P1" : "P2")
              << " | score: "          << pstate.score 
              << " | captured: "       << pstate.captured_cards
              << " | hand size: "      << pstate.hand.cards.size()
              << '\n';

    // Table state before the move
    std::cout << "Table cards (bitset): "
              << std::bitset<16>(to_u16(game_state.table.cards))
              << '\n';

    // Played card + targets
    std::cout << "Playing card: "      << rank_to_str(played_card)
              << " =     " << played_bits << "\n";

    std::cout << "Targets mask:         "      << target_bits
              << " -> "                << (is_single_capture ? "single capture"
                                                             : "multi-capture/addition")
              << '\n';

    std::cout << "Targets "            << (targets_exist ? "do" : "do not")
              << " exist on the table.\n";

    std::cout << "========================================================\n";
}

}