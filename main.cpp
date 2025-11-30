#include "cuarenta.h"
#include "movegen.h"
#include "bot.h"
#include <bitset>
#include <iomanip>

int main() {

    // Cuarenta::Hand hand1 = Cuarenta::generate_hand(game.deck);
    // Cuarenta::Hand hand2 = Cuarenta::generate_hand(game.deck);

    Cuarenta::Hand hand1 { {
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Five,
        Cuarenta::Rank::Six,
        Cuarenta::Rank::Seven,
        Cuarenta::Rank::Four,
    } };

    Cuarenta::Hand hand2 { {
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Four,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Two,
        Cuarenta::Rank::Five,
    } };

    Cuarenta::Game_State game { hand1, hand2 };

    game.players[0].hand.print_hand();
    game.players[1].hand.print_hand();
    
    game.to_move = Cuarenta::Player::P1;

    std::vector<Cuarenta::Move> moves { Cuarenta::generate_all_moves(game.table, game.players[0].hand) };

    std::cout << "All Possible Moves: " << '\n';
    for (size_t i = 0; i < moves.size(); i++) {

        std::bitset<16> x{Cuarenta::to_u16(moves[i].targets_mask)};
        std::cout << "Move #" << std::left << std::setw(2) << i+1 << ": " << x << '\n';

        Cuarenta::print_move(game, moves[i]);
    }

    std::cout << "======CURRENT BOARD=====\n";
    game.players[0].hand.print_hand();
    game.players[1].hand.print_hand();
    game.table.print_table();
    std::cout << "========================\n";

    int depth {10};
    std::cout << "Bot best moves (depth = " << depth << "): \n" 
              << Bot::minimax(game, depth, "") << '\n';

    return 0;
}
