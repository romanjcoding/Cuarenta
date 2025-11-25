#include "cuarenta.h"
#include "bot.h"

int main() {

    Cuarenta::Game1v1 game{};
    // Cuarenta::Hand hand1 = Cuarenta::generate_hand(game.deck);
    // Cuarenta::Hand hand2 = Cuarenta::generate_hand(game.deck);

    Cuarenta::Hand hand1 { {
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Five,
        Cuarenta::Rank::Six,
        Cuarenta::Rank::Seven,
        Cuarenta::Rank::Four,
        Cuarenta::Rank::Queen }
    };

    Cuarenta::Hand hand2 { {
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Four,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Two,
        Cuarenta::Rank::Five,
        Cuarenta::Rank::Seven } 
    };

    hand1.print_hand();
    hand2.print_hand();

    Cuarenta::play_card(
        Cuarenta::Move{ Cuarenta::RankMask{0b0000000000000001} }, 
        hand1,
        game.table); // A

    Cuarenta::play_card(
        Cuarenta::Move{ Cuarenta::RankMask{0b0000000000001000} }, 
        hand2,
        game.table); // 4

    Cuarenta::play_card(
        Cuarenta::Move{ Cuarenta::RankMask{0b0000000000100000} }, 
        hand1,
        game.table); // 6

    Cuarenta::play_card(
        Cuarenta::Move{ Cuarenta::RankMask{0b0000000001000000} }, 
        hand1,
        game.table); // 7

    Cuarenta::play_card(
        Cuarenta::Move{ Cuarenta::RankMask{0b0000000100000000} }, 
        hand1,
        game.table); // K


    game.table.print_table();
    // Bot::generate_all_moves(hand1, game.table);


    Cuarenta::play_card(
        Cuarenta::Move{ Cuarenta::RankMask{0b0000000000011001} }, 
        hand1,
        game.table); // 5

    // Cuarenta::play_card(hand1.cards[1], game.table, 3); // 5

    game.table.print_table();
    return 0;
}
