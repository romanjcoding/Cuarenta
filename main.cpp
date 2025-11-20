#include "cuarenta.h"

int main() {

    Cuarenta::Game1v1 game{};
    // Cuarenta::Hand hand1 = Cuarenta::generate_hand(game.deck);
    // Cuarenta::Hand hand2 = Cuarenta::generate_hand(game.deck);

    Cuarenta::Hand hand1 { {
        { Cuarenta::Rank::Ace },
        { Cuarenta::Rank::Five },
        { Cuarenta::Rank::Six },
        { Cuarenta::Rank::Seven },
        { Cuarenta::Rank::Four },
        { Cuarenta::Rank::Queen }
    } };

    Cuarenta::Hand hand2 { {
        { Cuarenta::Rank::Ace },
        { Cuarenta::Rank::Four },
        { Cuarenta::Rank::Three },
        { Cuarenta::Rank::Two },
        { Cuarenta::Rank::Five },
        { Cuarenta::Rank::Seven }
    } };

    hand1.print_hand();
    hand2.print_hand();

    Cuarenta::play_card(hand1.cards[0], game.table); // A
    Cuarenta::play_card(hand2.cards[1], game.table); // 4
    Cuarenta::play_card(hand1.cards[2], game.table); // 6
    Cuarenta::play_card(hand2.cards[5], game.table); // 7
    Cuarenta::play_card(hand1.cards[5], game.table); // Q

    game.table.print_table();

    Cuarenta::play_card(hand1.cards[1], game.table, 3); // 5

    game.table.print_table();
    return 0;
}
