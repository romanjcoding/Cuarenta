#include "cuarenta.h"
#include <random>
#include <algorithm>
#include <optional>

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
            d.cards.push_back(Card{r});
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(d.cards.begin(), d.cards.end(), g);
    return d;
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

bool remove_rank_if_present(std::vector<Card>& cards, const Card& card) {
    size_t prev_size = cards.size();
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
    return cards.size() < prev_size;
}

bool contains_multiple(const std::vector<Card>& cards_A, const std::vector<Card>& cards_B) {
    for (Card card_B : cards_B) {
        if (std::find(cards_A.begin(), cards_A.end(), card_B) == cards_A.end()) {
            return false;
        };
    }
    return true;
}

void sequence_waterfall(std::vector<Card>& cards, const Card& start_card) {
    Card current_card = start_card;
    while (current_card.rank_ != Rank::King) {
        current_card.rank_++;
        if (!remove_rank_if_present(cards, current_card)) { return; } 
    }
}

int play_card(const Card& card, 
               Table& table, 
               int capture_offset) {
    
    bool capture_made { false };
    int points_scored { 0 };
    
    if (capture_offset == 0) {
        if (remove_rank_if_present(table.cards, card)) { 

            capture_made = true;

            // caida, +2pts
            if (card.rank_ == table.last_played_card.rank_) { 
                points_scored += 2;
            }
            sequence_waterfall(table.cards, card);
        }
    }

    if (capture_offset != 0) {

        // Result = a1 + a2, 
        // Diff.  = a1 - a2
        // a1 = (R+D)/2
        // a2 = (R-D)/2        
        Card sum_1 {int_to_rank((rank_to_int(card.rank_) + capture_offset) / 2)};
        Card sum_2 {int_to_rank((rank_to_int(card.rank_) - capture_offset) / 2)};

        std::vector<Card> to_check { { sum_1, sum_2 } };

        if (contains_multiple(table.cards, to_check)) {
            capture_made = true;
            remove_rank_if_present(table.cards, sum_1);
            remove_rank_if_present(table.cards, sum_2);
            sequence_waterfall(table.cards, card);
        }
    }

    // limpia, +2 points
    if (table.cards.empty()) {
        points_scored += 2;
    }

    if (!capture_made) {
        table.cards.push_back(card);
        table.last_played_card = card;
    }

    return points_scored;
}
}