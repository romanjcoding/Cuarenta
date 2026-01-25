#include "cli.h"
#include "bot.h"
#include "cuarenta.h"
#include "timer.h"
#include "movegen.h"

#include <assert.h>
#include <exception>
#include <vector>
#include <iostream>
#include <iomanip>
#include <bit>

struct Data {
    Cuarenta::Player player;
    int num_captured_cards{};
    int delta_num_captured_cards{};
    int delta_score{};
    int round_score_cl{};
    int num_table_cards{};
    bool is_updated {};
};

int main() {
    
    int num_iter { 20 };
    std::vector<Data> data {};

    for (int game_iter{}; game_iter < num_iter; game_iter++) {
        std::cerr << "Starting Game #" << game_iter + 1 << std::endl;

        Bot::Bot botp1 {100};
        Bot::Bot botp2 {100};
        Cuarenta::Game_State game{};

        data.push_back( Data{.player = Cuarenta::Player::P1, .is_updated = false} );
        data.push_back( Data{.player = Cuarenta::Player::P2, .is_updated = false} );

        botp1.update_from_hand(Cuarenta::state_for(game, Cuarenta::Player::P1).hand);
        botp2.update_from_hand(Cuarenta::state_for(game, Cuarenta::Player::P2).hand);

        auto finalize_pending_data = [&]() {
        for (auto& d : data) {
            if (!d.is_updated) {
                d.is_updated = true;
                d.num_captured_cards = Cuarenta::state_for(game, d.player).num_captured_cards;
                d.delta_num_captured_cards =
                    Cuarenta::state_for(game, d.player).num_captured_cards
                    - d.delta_num_captured_cards;
                d.delta_score =
                    Cuarenta::state_for(game, d.player).score
                    - d.delta_score;
            } } };

        while (true) {

            auto& current  { Cuarenta::current_player_state(game) };
            auto& opponent { Cuarenta::opposing_player_state(game) };
            auto& current_bot  { (game.to_move == Cuarenta::Player::P1) ? botp1 : botp2 };
            auto& opposing_bot { (game.to_move == Cuarenta::Player::P1) ? botp2 : botp1 };

            if (current.score >= 40 || opponent.score >= 40) { 
                finalize_pending_data();
                break; 
            } 

            if (current.hand.cards.empty() && opponent.hand.cards.empty()) {

                finalize_pending_data();

                if (game.deck.cards.empty()) {
                    Cuarenta::update_captured_cards(game);
                    game.deck = Cuarenta::Deck{true};
                    game.table.reset();
                    botp1.reset_probabilities();
                    botp2.reset_probabilities();
                }

                current.hand  = game.deck.draw_hand();
                opponent.hand = game.deck.draw_hand();
                current_bot.update_from_hand(current.hand);
                opposing_bot.update_from_hand(opponent.hand);
                game.table.last_played_card = Cuarenta::Rank::Invalid;

                data.push_back(
                    Data{.delta_num_captured_cards = current.num_captured_cards,
                         .delta_score = current.score,
                         .player = (game.to_move == Cuarenta::Player::P1) ? Cuarenta::Player::P1 : Cuarenta::Player::P2,
                         .num_table_cards = std::popcount(to_u16(game.table.cards)),
                         .is_updated = false});

                data.push_back(
                    Data{.delta_num_captured_cards = opponent.num_captured_cards,
                         .delta_score = opponent.score,
                         .player = (game.to_move == Cuarenta::Player::P1) ? Cuarenta::Player::P2 : Cuarenta::Player::P1,
                         .num_table_cards = std::popcount(to_u16(game.table.cards)),
                         .is_updated = false});
            }

            auto move { Bot::choose_best_move(current_bot, game, 10) };
            Cuarenta::make_move_in_place(game, move);
            opposing_bot.update_from_move(move);
            game.advance_turn();
        }
    }

    std::cout << "Player, Num Table Cards, Captured Cards, Δ Captured Cards, Δ Score" << '\n';
    for (const auto& d : data) {
        std::cout
            << ((d.player == Cuarenta::Player::P1) ? "P1" : "P2")
            << ", " << d.num_table_cards
            << ", " << d.num_captured_cards
            << ", " << d.delta_num_captured_cards
            << ", " << d.delta_score << '\n';
    }
}