#include "cli.h"

#include "cli_parse.h"
#include "cli_render.h"
#include "cuarenta.h"
#include "movegen.h"
#include "bot.h"
#include "game_state.h"
#include "rank.h"

#include <algorithm>
#include <bit>
#include <chrono>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace cli {

std::string describe_move_targets(const Cuarenta::Move &mv) {

    if (to_u16(mv.targets_mask) == 0) {
        return {};
    }

    std::vector<Cuarenta::Rank> targets { Cuarenta::mask_to_vector(mv.targets_mask) };

    std::vector<std::string> parts;
    parts.reserve(targets.size());
    for (const auto &r : targets) {
        parts.push_back(Cuarenta::rank_to_str(r));
    }
    std::string combo;
    for (size_t i{}; i < parts.size(); i++) {
        combo += parts[i];
        if (i + 1 < parts.size()) combo += " + ";
    }
    std::string played = Cuarenta::rank_to_str(mv.get_played_rank());
    return played + " = " + combo;
}

std::optional<size_t> first_index_of_rank(const std::vector<Cuarenta::Rank>& cards,
                                               Cuarenta::Rank r) {
    for (size_t i{}; i < cards.size(); i++) {
        if (cards[i] == r) { return i; }
    }
    return std::nullopt;
}

void apply_move_with_animation(Cuarenta::Game_State& game,
                               const Cuarenta::Move& mv,
                               View view) {
                                
    Cuarenta::Rank played  { mv.get_played_rank() };

    auto temp_game { game };

    const auto& p1 { Cuarenta::state_for(game, Cuarenta::Player::P1) };
    const auto& p2 { Cuarenta::state_for(game, Cuarenta::Player::P2) };

    const auto& bottom { (view == View::P1Fixed) ? p1
                     : Cuarenta::current_player_state(game) };
    const auto& top    { (view == View::P1Fixed) ? p2
                     : Cuarenta::opposing_player_state(game) };

    auto current_is_on_bottom = [&] {
        if (view == View::ByTurn) { return true; }
        return (game.to_move == Cuarenta::Player::P1);
    };

    {
        const bool on_bottom   { current_is_on_bottom() };
        const auto& hand_cards { on_bottom ? bottom.hand.cards : top.hand.cards };
        auto idx = first_index_of_rank(hand_cards, played);

        if (idx) {
            RenderOpts opts;
            if (on_bottom) {
                opts.bottom_hi_index = idx;
                opts.bottom_color = ansi::yellow;
            } else {
                opts.top_hi_index = idx;
                opts.top_color = ansi::yellow;
            }
            print_game_state(temp_game, Cuarenta::mask_to_vector(temp_game.table.cards), view, opts);
            sleep_ms(700);
        }

        auto played_rank { mv.get_played_rank() };

        {
            RenderOpts opts;

            auto visible_table { Cuarenta::mask_to_vector(temp_game.table.cards) };
            auto it { std::ranges::lower_bound(visible_table, played_rank) };
            visible_table.insert(it, played_rank);
            opts.table_idx_to_highlight = { static_cast<size_t>( std::distance(
                visible_table.begin(), it)) };
            opts.table_color = ansi::yellow;

            temp_game.table.cards = temp_game.table.cards | to_mask(played_rank);

            Cuarenta::remove_card_from_hand(Cuarenta::current_player_state(temp_game).hand, played_rank);

            print_game_state(temp_game, visible_table, view, opts);
            sleep_ms(700);
            
            const bool is_addition { !std::has_single_bit(to_u16(mv.targets_mask)) };
            const bool is_capture  { is_addition || Cuarenta::contains_ranks(game.table.cards, to_mask(played_rank)) };

            // Caida matching
            if (is_capture && !is_addition) {
                for (size_t i{}; i < visible_table.size(); i++) {
                    if (visible_table[i] == played_rank) {
                        opts.table_idx_to_highlight.push_back(i);
                    }
                }
            }
            // Addition matching
            else {
                const auto addition_mask { mv.targets_mask & ~Cuarenta::to_mask(played_rank) };
                const auto vec { Cuarenta::mask_to_vector(addition_mask) };
                for (size_t i{}; i < visible_table.size(); i++) {
                    if (std::ranges::find(vec, visible_table[i]) != vec.end()) {
                        opts.table_idx_to_highlight.push_back(i);
                    }
                }
            }

            print_game_state(temp_game, visible_table, view, opts);
            sleep_ms(500);

            // Waterfall matching
            if (is_capture) {
                int num_waterfalled{};
                while (played_rank != Cuarenta::Rank::King) {
                    played_rank++;
                    if (contains_ranks(game.table.cards, to_mask(played_rank))) { 
                        auto iter { std::ranges::find(visible_table, played_rank) };
                        opts.table_idx_to_highlight.push_back(
                            static_cast<size_t>(std::distance(visible_table.begin(), iter)));
                            
                        opts.table_color = ansi::waterfall(static_cast<size_t>(++num_waterfalled));
                        print_game_state(temp_game, visible_table, view, opts);
                        sleep_ms(500 - 35 * num_waterfalled);
                    }
                    else { break; }
                }
            }
        }

    }
    Cuarenta::make_move_in_place(game, mv);
    print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view);
}

Bot::Bot bot_selection_screen() {
    while (true) {
        clear_screen();
        print_bot_select_banner();
        auto input { read_input() };
        if (input.bot.has_value()) { return input.bot.value(); }
    }
}

size_t choose_move_ai(Cuarenta::Game_State& game, Bot::Bot bot, int depth) {
    auto evals = Bot::evaluate_all_moves_mc(bot, game, depth);
    if (evals.empty()) { return 0; }
    size_t best_idx{};
    double best_val = evals.front().eval;
    for (size_t i = 0; i < evals.size(); i++) {
        if (evals[i].eval > best_val) {
            best_val = evals[i].eval;
            best_idx = i;
        }
    }
    return best_idx;
}

void run_game(std::optional<Bot::Bot> bot) {

    Cuarenta::Game_State game{};

    bool versus_bot{};
    if (bot.has_value()) { versus_bot = true; }
    else                 { versus_bot = false; }

    std::cout << "Starting a new "
              << (versus_bot ? "human vs computer" : "human vs human")
              << " game...\n";
    bool exit_game = false;

    while (true) {
        auto& current  = Cuarenta::current_player_state(game);
        auto& opponent = Cuarenta::opposing_player_state(game);

        if (current.score >= 40 || opponent.score >= 40) { break; }

        if (current.hand.cards.empty() && opponent.hand.cards.empty()) {
            game.table.last_played_card = Cuarenta::Rank::Invalid;
            if (game.deck.cards.empty()) {
                Cuarenta::update_captured_cards(game);
                game.deck = Cuarenta::Deck{true};
            } else {
                current.hand  = game.deck.draw_hand();
                opponent.hand = game.deck.draw_hand();
            }
        }

        bool current_is_human = true;
        if (versus_bot && game.to_move == Cuarenta::Player::P2) {
            current_is_human = false;
        }

        View view { versus_bot ? View::P1Fixed : View::ByTurn };

        if (current_is_human) {
            print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view);
        }

        auto moves { Cuarenta::generate_all_moves(game) };
        size_t chosen{};

        if (current_is_human) {
            while (true) {
                std::cout << "\n\nEnter move (e.g., '5' or '5 = 3 + 2'), "
                             "'help' for commands, or 'quit' to end the game: ";

                auto input { read_input() };

                if (input.help) {
                    clear_screen();
                    print_help_banner();
                    std::cout << "\nAvailable moves:\n";
                    for (size_t i{}; i < moves.size(); i++) {
                        const auto& mv { Cuarenta::Move{moves.at(i)} };
                        std::string desc = describe_move_targets(mv);
                        if (!desc.empty()) std::cout << "  " << desc << "\n";
                        else std::cout << "  " << Cuarenta::rank_to_str(mv.get_played_rank()) << "\n";
                    }
                    continue;
                }

                if (input.quit) {
                    exit_game = true;
                    break;
                }

                if (input.move.has_value()) {
                    const auto move { input.move.value() };
                    size_t it = moves.find(move.targets_mask);
                    if (it != moves.capacity()) {
                        chosen = it;
                        break;
                    }
                }

                flash_invalid_input(game, view);
                std::cout << "Unrecognised move. Please enter one of the moves listed above, "
                             "'help', or 'quit'.\n";
            }
        } else if (bot.has_value()) {
            print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view);
            chosen = choose_move_ai(game, bot.value(), 10);
        }

        if (exit_game) { break; }

        apply_move_with_animation(game, Cuarenta::Move{moves.at(chosen)}, view);
        game.advance_turn();
    }

    if (exit_game) {
        std::cout << "\nYou have quit the game before completion.\n";
        return;
    }

    // Final scoring and result
    std::cout << "\nFinal scores:\n";
    const auto& p1score { Cuarenta::state_for(game, Cuarenta::Player::P1).score };
    const auto& p2score { Cuarenta::state_for(game, Cuarenta::Player::P2).score };

    std::cout << "Player 1: " << p1score << "\n";
    std::cout << "Player 2: " << p2score << "\n";
    if (p1score > p2score) {
        std::cout << "Player 1 wins!\n";
    } else if (p2score > p1score) {
        std::cout << "Player 2 wins!\n";
    } else {
        std::cout << "It's a tie!\n";
    }
}

int run_cli() {
    try {
        clear_screen();
        print_menu_banner();
        while (true) {

            auto input { read_input() };

            if (input.help) {
                clear_screen();
                print_help_banner();
            }

            else if (input.play_human) {
                run_game(std::nullopt);
                std::cout << "\nGame finished. Back to main menu.\n";
                sleep_ms(1500);
                clear_screen();
                print_menu_banner();
            }

            else if (input.play_bot) {
                Bot::Bot bot { bot_selection_screen() };
                run_game(bot);
                std::cout << "\nGame finished. Back to main menu.\n";
            }

            else if (input.quit) {
                std::cout << "Goodbye!\n";
                break;
            }

            else { 
                std::cout << "Unknown command. Type 'help' for a list of commands.\n";
            }
        }
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}

} // namespace cli
