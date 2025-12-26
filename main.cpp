#include "cuarenta.h"
#include "movegen.h"
#include "bot.h"
#include <bitset>
#include <iomanip>
#include <utility>
#include <cassert>

void print_game(const Cuarenta::Game_State& game) {
    for (const Cuarenta::Player_State& player : game.players) {
        player.hand.print_hand();
    }
    game.table.print_table();
}

Cuarenta::Move eval_and_make_move(Cuarenta::Game_State& game, Bot::Bot& bot) {

    print_game(game);

    auto moves    { Bot::evaluate_all_moves_mc(bot, game, 10) };
    auto max_move { moves.front() };
    for (const Bot::Move_Eval& mev : moves) {
        if (mev.value > max_move.value) {
            max_move = mev;
        }
        const double ci999 = 3.27 * (mev.std_dev / std::sqrt(bot.num_mc_iters_));
        std::cout << "Analyzed move: "
            << std::left  << std::setw(16) << Cuarenta::mask_to_str(mev.move.targets_mask)
            << "  eval " << std::showpos << std::fixed << std::setprecision(5) << mev.value << std::noshowpos
            << "  ± " << std::fixed << std::setprecision(5) << ci999
            << " (99.9% CI)"
            << "  [stddev=" << std::fixed << std::setprecision(5) << mev.std_dev << "]\n";
    }
    std::cout << std::endl;
    return Cuarenta::make_move_in_place(game, max_move.move).move;
}

int main(int argv, char** argc) {

    // Cuarenta::Hand hand1 { {
    //     Cuarenta::Rank::Ace,
    //     Cuarenta::Rank::Two,
    //     Cuarenta::Rank::Three,
    //     Cuarenta::Rank::Ace,
    //     Cuarenta::Rank::Six,
    // } };

    // Cuarenta::Hand hand2 { {
    //     Cuarenta::Rank::Seven,
    //     Cuarenta::Rank::Three,
    //     Cuarenta::Rank::Three,
    //     Cuarenta::Rank::Six,
    //     Cuarenta::Rank::Seven,
    // } };

        Cuarenta::Hand hand1 { {
        Cuarenta::Rank::Four,
        Cuarenta::Rank::Four,
        Cuarenta::Rank::Queen,
        Cuarenta::Rank::Queen,
        Cuarenta::Rank::Seven,
    } };

    Cuarenta::Hand hand2 { {
        Cuarenta::Rank::Seven,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Three,
        Cuarenta::Rank::Six,
        Cuarenta::Rank::Seven,
    } };

    Cuarenta::Game_State game {hand1, hand2};

    // int NUM_ITER  { 10000 }; // 1000000, ~37 min
    Bot::Bot bot1 { Bot::Bot{Bot::BotType::ROBOT, 100000} };
    Bot::Bot bot2 { Bot::Bot{Bot::BotType::ROBOT, 100000} };

    Cuarenta::Move last_played_move { .targets_mask = to_mask(Cuarenta::Rank::Invalid) };

    bot1.initialize_probabilities(game.players[0].hand);
    bot2.initialize_probabilities(game.players[1].hand);

    for (int _{}; _ < 5; _++) {
        
        last_played_move = Cuarenta::Move{ eval_and_make_move(game, bot1) };
        bot2.update_probabilities(last_played_move);
        game.advance_turn();

        last_played_move = eval_and_make_move(game, bot2);
        bot1.update_probabilities(last_played_move);
        game.advance_turn();
    }

    assert(bot1.hand_probabilities_.p1 == bot2.hand_probabilities_.p1);
    print_game(game);

    return 0;
}
