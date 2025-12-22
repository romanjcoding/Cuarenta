#include "cuarenta.h"
#include "movegen.h"
#include "bot.h"
#include <bitset>
#include <iomanip>
#include <utility>

int main() {

    // Cuarenta::Hand hand1 = Cuarenta::generate_hand(game.deck);
    // Cuarenta::Hand hand2 = Cuarenta::generate_hand(game.deck);

    Cuarenta::Hand hand1 { {
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Two,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Ace,
        Cuarenta::Rank::Six,
    } };

    Cuarenta::Hand hand2 { {
        Cuarenta::Rank::Seven,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Six,
        Cuarenta::Rank::Seven,
    } };

    Cuarenta::Game_State game{ hand1, hand2 };
    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    int NUM_ITER  { 1000 };
    auto moves    { Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 10) };
    auto max_move { moves.front() };

    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    game.advance_turn();
    std::cout << std::endl;

    for (Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();

    moves = Bot::evaluate_all_moves_mc(Bot::Bot{Bot::BotType::ROBOT, NUM_ITER}, game, 9);
    max_move = moves.front();
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(NUM_ITER));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }

    game = Cuarenta::make_move(game, max_move.move);
    std::cout << std::endl;

    for (auto& player : game.players) { 
        std::cout << player.score << '\n';
        player.hand.print_hand(); 
        std::cout << '\n';
    }
    game.table.print_table();

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
