#include "cuarenta.h"
#include "movegen.h"
#include "cli.h"
#include "bot.h"
#include <bitset>
#include <iomanip>
#include <utility>

int main() {

    // Cuarenta::Hand hand1 = Cuarenta::generate_hand(game.deck);
    // Cuarenta::Hand hand2 = Cuarenta::generate_hand(game.deck);

    Cuarenta::Hand hand1 { {
        Cuarenta::Rank::Four,
        Cuarenta::Rank::Jack,
        Cuarenta::Rank::Six,
        Cuarenta::Rank::Five,
        Cuarenta::Rank::Seven,
    } };

    Cuarenta::Hand hand2 { {
        Cuarenta::Rank::Two,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Ace,
    } };

    Cuarenta::Game_State game{};
    for (Cuarenta::Player_State& player : game.players) {
        player.hand = game.deck.draw_hand();
        player.hand.print_hand();
    }
    auto moves { Bot::evaluate_all_moves_mc(Bot::BotType::ROBOT, game, 10) };
    for (Bot::Move_Eval& mev : moves) {
        std::cout << "Analyzed move: "
        << std::left << std::setw(15) << Cuarenta::mask_to_str(mev.move.targets_mask)
        << " eval = "
        << ((mev.value > 0) ? "+" : "")
        << mev.value << "\n";
    }
    // print_move(game, move.move);
    // std::cout << "\n\n\n";
    // game = Cuarenta::make_move(game, move.move);
    // game.advance_turn();

    // int depth { 10 };
    
    // run_cuarenta_cli(game, depth);
    
    // game.to_move = Cuarenta::Player::P1;
    // int depth { 10 };

    // auto print_best_move = [&](Cuarenta::Game_State& game, int depth) {
    //     auto [eval, move] = Bot::choose_best_move(game, depth);

    //     std::cout << "Bot best move (" 
    //             << (game.to_move == Cuarenta::Player::P1 ? "P1" : "P2")
    //             << ", depth = " << depth
    //             << ", eval = " << ((eval >= 0) ? "+" : "") << eval << ")\n"
    //             << "  play:     " << Cuarenta::mask_to_str(move.targets_mask) << '\n'
    //             << "  bitmask:  " << std::bitset<16>{to_u16(move.targets_mask)} << "\n\n";

    //     game = Cuarenta::make_move(game, move);
    // };

    // for (int turn = 0; turn < 10; turn++) {
    //     print_best_move(game, depth);
    //     game.to_move = (game.to_move == Cuarenta::Player::P1)
    //                 ? Cuarenta::Player::P2
    //                 : Cuarenta::Player::P1;
    // }
    
    // std::vector<Cuarenta::Move> moves { Cuarenta::generate_all_moves(game.table, game.players[0].hand) };

    // std::cout << "All Possible Moves: " << '\n';
    // for (size_t i = 0; i < moves.size(); i++) {

    //     std::bitset<16> x{Cuarenta::to_u16(moves[i].targets_mask)};
    //     std::cout << "Move #" << std::left << std::setw(2) << i+1 << ": " << x << '\n';

    //     Cuarenta::print_move(game, moves[i]);
    // }

    // std::cout << "======CURRENT BOARD=====\n";
    // game.players[0].hand.print_hand();
    // game.players[1].hand.print_hand();
    // game.table.print_table();
    // std::cout << "========================\n";

    return 0;
}
