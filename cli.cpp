#include "cli.h"

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

std::optional<Cuarenta::Rank> try_parse_rank_token(const std::string& token) {

    if (token.empty()) { return std::nullopt; }

    std::string normalized;
    normalized.reserve(token.size());
    for (char ch : token) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }
    for (int i{1}; i <= Cuarenta::NUM_RANKS; i++) {
        Cuarenta::Rank r { Cuarenta::int_to_rank(i) };
        std::string rs   { Cuarenta::rank_to_str(r) };
        std::string rs_upper;
        for (char c : rs) {
            rs_upper += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        if (rs_upper == normalized) {
            return r;
        }
        bool numeric = !normalized.empty();
        for (char c : normalized) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                numeric = false;
                break;
            }
        }
        if (numeric) {
            try {
                int value = std::stoi(normalized);
                if (value == i) {
                    return r;
                }
            } catch (...) {
            }
        }
    }
    return std::nullopt;
}

InputData read_input() {

    std::string input;
    if (!std::getline(std::cin, input)) {
        return InputData { .err = true };
    }

    auto start { input.find_first_not_of(" \t") };
    auto end   { input.find_last_not_of(" \t") };
    if (end == std::string::npos) { return InputData { .err = true }; }
    input = input.substr(start, end - start + 1);

    if (input == "help") { return InputData { .help = true }; }
    if (input == "quit") { return InputData { .quit = true }; }

    std::string lhs;
    std::string rhs;
    size_t eq_pos = input.find('=');
    if (eq_pos != std::string::npos) {
        lhs = input.substr(0, eq_pos);
        rhs = input.substr(eq_pos + 1);
    } else {
        lhs = input;
    }

    std::optional<Cuarenta::Rank> played_rank { try_parse_rank_token(lhs) };
    if (!played_rank.has_value()) { return InputData { .err = true }; }

    return InputData { .err = true };
}

void print_cards_box(const std::vector<Cuarenta::Rank>& cards,
                     bool is_empty,
                     std::optional<size_t> highlight_index = std::nullopt,
                     Cuarenta::RankMask highlight_mask = Cuarenta::to_mask(0),
                     std::string_view highlight_color  = ansi::reset,
                     bool error_flash                  = false)
{
    const std::string top    = "╭─────╮";
    const std::string bottom = "╰─────╯";

    auto begin_color = [&](bool hi) {
        if (error_flash) {
            std::cout << ansi::red;
            return;
        }
        if (hi) {
            std::cout << highlight_color;
            return;
        }
        if (is_empty) {
            std::cout << ansi::dim << ansi::gray;
            return;
        }
        std::cout << ansi::white;
    };

    auto end_color = []() {
        std::cout << ansi::reset;
    };

    auto is_hi = [&](size_t idx) -> bool {
        if (highlight_index && *highlight_index == idx) {
            return true;
        }
        if (!is_empty) {
            Cuarenta::Rank r = cards[idx];
            if (Cuarenta::contains_ranks(highlight_mask, Cuarenta::to_mask(r))) {
                return true;
            }
        }
        return false;
    };

    // Top
    for (size_t i{}; i < cards.size(); i++) {
        bool hi { is_hi(i) };
        begin_color(hi);
        std::cout << top;
        end_color();
        std::cout << ' ';
    }
    if (!cards.empty()) std::cout << '\n';

    // Padding line
    for (size_t i{}; i < cards.size(); i++) {
        bool hi { is_hi(i) };
        begin_color(hi);
        std::cout << "│     │";
        end_color();
        std::cout << ' ';
    }
    std::cout << '\n';

    // Rank line
    for (size_t i{}; i < cards.size(); i++) {
        bool hi { is_hi(i) };
        begin_color(hi);
        std::string label { " " };
        if (!is_empty) {
            std::string rs { Cuarenta::rank_to_str(cards[i]) };
            if (!rs.empty()) { label = rs.substr(0, 1); }
        }
        std::cout << "│  " << label << "  │";
        end_color();
        std::cout << ' ';
    }
    std::cout << '\n';

    // Padding line
    for (size_t i = 0; i < cards.size(); i++) {
        bool hi = is_hi(i);
        begin_color(hi);
        std::cout << "│     │";
        end_color();
        std::cout << ' ';
    }
    std::cout << '\n';

    // Bottom
    for (size_t i = 0; i < cards.size(); i++) {
        bool hi = is_hi(i);
        begin_color(hi);
        std::cout << bottom;
        end_color();
        std::cout << ' ';
    }
    if (!cards.empty()) std::cout << '\n';
}

std::vector<std::string> build_cards_box_lines(
    const std::vector<Cuarenta::Rank>& cards,
    bool is_empty,
    std::optional<size_t> highlight_index,
    std::vector<size_t> table_idx_to_highlight,
    std::string_view highlight_color,
    bool error_flash)
{
    std::vector<std::string> lines(CARD_LINES_H);

    // Border templates
    const std::string top    = "╭─────╮";
    const std::string bottom = "╰─────╯";

    auto prefix = [&](bool is_highlighted) -> std::string {
        if (error_flash)    return std::string(ansi::red);
        if (is_highlighted) return std::string(highlight_color);
        if (is_empty)       return std::string(ansi::dim) + std::string(ansi::gray);
        return std::string(ansi::white);
    };
    auto suffix = []() -> std::string {
        return std::string(ansi::reset);
    };

    auto is_highlighted = [&](size_t idx) -> bool {
        if (highlight_index && *highlight_index == idx) { return true; }
        if (!is_empty) {
            return std::ranges::find(table_idx_to_highlight, idx) != table_idx_to_highlight.end();
        }
        return false;
    };

    for (size_t i{}; i < cards.size(); i++) {
        bool hi = is_highlighted(i);

        lines[0] += prefix(hi) + top        + suffix() + " ";
        lines[1] += prefix(hi) + "│     │"  + suffix() + " ";

        std::string label = " ";
        if (!is_empty) {
            std::string rs = Cuarenta::rank_to_str(cards[i]);
            if (!rs.empty()) label = rs.substr(0, 1);
        }
        lines[2] += prefix(hi) + "│  " + label + "  │" + suffix() + " ";

        lines[3] += prefix(hi) + "│     │"  + suffix() + " ";
        lines[4] += prefix(hi) + bottom     + suffix() + " ";
    }
    return lines;
}

void print_table_framed(const std::vector<Cuarenta::Rank>& table_cards,
                        const RenderOpts& opt) {

    static constexpr int FRAME_PAD { 1 };

    const size_t inner_w { TABLE_INNER_W };
    const size_t frame_w { inner_w + 2 * FRAME_PAD };

    auto repeat_utf8 = [](std::string_view s, size_t n) {
        std::string out;
        out.reserve(s.size() * n);
        for (size_t i{}; i < n; i++) { out += s; }
        return out;
    };

    auto build_hline = [&](std::string_view left, std::string_view right) {
        return std::string(left) + repeat_utf8("─", frame_w) + std::string(right);
    };

    auto print_row = [&](std::string_view content) {
        std::cout << "│" << std::string(FRAME_PAD, ' ')
                  << content
                  << std::string(FRAME_PAD, ' ')
                  << "│\n";
    };

    const std::string top = build_hline("┌", "┐");
    const std::string bot = build_hline("└", "┘");

    const size_t num_cards { table_cards.size() };

    std::cout << top << "\n";

    std::string header { "TABLE" };
    std::string headerbuf(inner_w, ' ');

    for (size_t i{}; i < header.size() && i < headerbuf.size(); i++) {
        headerbuf[i] = header[i];
    }

    print_row(headerbuf);

    // Empty table
    if (num_cards == 0) {
        for (int row{}; row < CARD_LINES_H; row++) {
            std::string content(inner_w, ' ');
            if (row == 2) {
                const std::string msg = "-- empty table --";
                const size_t start = (inner_w > msg.size()) ? (inner_w - msg.size()) / 2 : 0;
                for (size_t i{}; i < msg.size() && start + i < content.size(); i++) {
                    content[start + i] = msg[i];
                }
                std::cout << "│" << std::string(FRAME_PAD, ' ')
                          << ansi::dim << ansi::gray << content << ansi::reset
                          << std::string(FRAME_PAD, ' ')
                          << "│\n";
            } else {
                print_row(content);
            }
        }
        std::cout << bot << "\n";
        return;
    }

    auto card_lines = build_cards_box_lines(
        table_cards, false,
        std::nullopt,     
        opt.table_idx_to_highlight,
        opt.table_color,
        opt.error_flash
    );

    const size_t cards_w   { std::min<size_t>(inner_w, num_cards * CARD_CELL_W) };
    const size_t left_pad  { (inner_w - cards_w) / 2 };
    const size_t right_pad { inner_w - cards_w - left_pad };

    for (size_t row{}; row < static_cast<size_t>(CARD_LINES_H); row++) {
        std::cout << "│" << std::string(FRAME_PAD, ' ')
                  << std::string(left_pad, ' ')
                  << card_lines[row]
                  << std::string(right_pad, ' ')
                  << std::string(FRAME_PAD, ' ')
                  << "│\n";
    }

    std::cout << bot << "\n";
}

Bot::Bot bot_selection_screen() {
    clear_screen();
    std::cout << "============================================================\n";
    std::cout << "                    Bot Selection Screen                    \n";
    std::cout << "============================================================\n";
    while (true) {
        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }
    }
    sleep_ms(1000);
    return Bot::BOT_CHILD;
}

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

int find_move_by_input(const std::string& raw_input,
                       const Cuarenta::MoveList& moves)
{
    auto start { raw_input.find_first_not_of(" \t") };
    auto end   { raw_input.find_last_not_of(" \t") };
    if (start == std::string::npos) { return -1; }
    std::string input = raw_input.substr(start, end - start + 1);

    std::string lhs;
    std::string rhs;
    size_t eq_pos = input.find('=');
    if (eq_pos != std::string::npos) {
        lhs = input.substr(0, eq_pos);
        rhs = input.substr(eq_pos + 1);
    } else {
        lhs = input;
    }

    std::optional<Cuarenta::Rank> played { try_parse_rank_token(lhs) };

    // Parse targets if any
    Cuarenta::RankMask targets_mask = Cuarenta::to_mask(0);
    if (!rhs.empty()) {
        std::vector<Cuarenta::Rank> target_ranks;
        std::string token;
        auto finish_token = [&](const std::string& tok) -> bool {
            auto b = tok.find_first_not_of(" \t");
            auto e = tok.find_last_not_of(" \t");
            if (b == std::string::npos) { return false; }
            auto t { try_parse_rank_token(tok.substr(b, e - b + 1)) };
            if (!t) { return false; }
            target_ranks.push_back(t.value());
            return true;
        };
        for (char ch : rhs) {
            if (ch == '+') {
                if (!finish_token(token)) return -1;
                token.clear();
            } else {
                token += ch;
            }
        }
        if (!token.empty()) {
            if (!finish_token(token)) return -1;
        }
        for (auto r : target_ranks) {
            targets_mask = targets_mask | Cuarenta::to_mask(r);
        }
    }

    int fallback = -1;
    for (size_t i{}; i < moves.size; i++) {

        const auto& mv = moves.moves[i];
        auto addition_mask { mv.targets_mask & ~to_mask(mv.get_played_rank()) };

        if (mv.get_played_rank() != played)  { continue; }
        if (targets_mask == addition_mask)   { return static_cast<int>(i); }
        if (rhs.empty()) {
            fallback = static_cast<int>(i);
        }
    }
    return fallback;
}

void print_scoreboard(const Cuarenta::Game_State& game) {

    const auto& p1 { Cuarenta::state_for(game, Cuarenta::Player::P1) };
    const auto& p2 { Cuarenta::state_for(game, Cuarenta::Player::P2) };

    auto arrow = [&](Cuarenta::Player p) {
        return (game.to_move == p) ? "▶" : " ";
    };

    std::cout <<
      "┌──────────────────────────────────────────────┐\n"
      "│                  SCOREBOARD                  │\n"
      "├───────────────┬──────────┬──────────┬────────┤\n"
      "│ Player        │ Score    │ Captured │ Turn   │\n"
      "├───────────────┼──────────┼──────────┼────────┤\n";
    std::cout << "│ Player 1      │ "
              << std::setw(6) << p1.score << "   │ "
              << std::setw(6) << p1.num_captured_cards << "   │  "
              << arrow(Cuarenta::Player::P1) << "     │\n";
    std::cout << "│ Player 2      │ "
              << std::setw(6) << p2.score << "   │ "
              << std::setw(6) << p2.num_captured_cards << "   │  "
              << arrow(Cuarenta::Player::P2) << "     │\n";
    std::cout <<
      "└───────────────┴──────────┴──────────┴────────┘\n\n";
}

void print_game_state(const Cuarenta::Game_State& game,
                      const std::vector<Cuarenta::Rank>& visible_table,
                      View view = View::ByTurn,
                      const RenderOpts& opt = {}) {
    clear_screen();
    print_scoreboard(game);
    
    if (!opt.banner.empty()) {
        if (opt.banner_bold) { std::cout << ansi::bold; }
        std::cout << opt.banner_color << opt.banner << ansi::reset << "\n\n";
    }

    // Determine which player appears on bottom and top
    const auto& p1 { Cuarenta::state_for(game, Cuarenta::Player::P1) };
    const auto& p2 { Cuarenta::state_for(game, Cuarenta::Player::P2) };

    const auto& bottom { (view == View::P1Fixed) ? p1 : Cuarenta::current_player_state(game) };
    const auto& top    { (view == View::P1Fixed) ? p2 : Cuarenta::opposing_player_state(game) };

    print_cards_box(top.hand.cards, true,
                    opt.top_hi_index, Cuarenta::to_mask(0),
                    opt.top_color, opt.error_flash);
    std::cout << "\n\n\n";
    print_table_framed(visible_table, opt);
    std::cout << "\n\n\n";
    print_cards_box(bottom.hand.cards, false,
                    opt.bottom_hi_index, Cuarenta::to_mask(0),
                    opt.bottom_color, opt.error_flash);

    std::cout << std::flush;
}

void flash_invalid_input(const Cuarenta::Game_State& game, View view) {
    RenderOpts red;
    red.error_flash = true;

    print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view, red);
    sleep_ms(70);
    print_game_state(game, Cuarenta::mask_to_vector(game.table.cards) ,view);
    sleep_ms(70);
    print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view, red);
    sleep_ms(70);
    print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view);
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
                        auto it { std::ranges::find(visible_table, played_rank) };
                        opts.table_idx_to_highlight.push_back(
                            static_cast<size_t>(std::distance(visible_table.begin(), it)));
                            
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

void show_banner() {
    std::cout << "============================================================\n";
    std::cout << "                  Welcome to CUARENTA CLI                   \n";
    std::cout << "============================================================\n";
    std::cout << "Cuarenta is a traditional fishing card game from Ecuador.\n";
    std::cout << "Reach 40 points before your opponent to win.\n";
    std::cout << "Type 'help' to see available commands.\n\n";
}

void show_help() {
    std::cout << "============================================================\n";
    std::cout << "                  Welcome to the Help Menu                  \n";
    std::cout << "============================================================\n";
    std::cout << "\nAvailable commands:\n";
    std::cout << "  play human   : start a two–player (human vs human) game\n";
    std::cout << "  play bot     : start a game against the computer\n";
    std::cout << "  help         : display this help message\n";
    std::cout << "  quit         : exit the program\n\n";
}

size_t choose_move_ai(Cuarenta::Game_State& game, int depth) {
    auto evals = Bot::evaluate_all_moves_mc(Bot::BOT_CHILD, game, depth);
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

void run_game(std::optional<Bot::Bot> bot)
{
    Cuarenta::Game_State game{};

    bool versus_bot{};
    if (bot) { versus_bot = true; }
    else     { versus_bot = false; }
    

    std::cout << "Starting a new "
              << (versus_bot ? "human vs computer" : "human vs human")
              << " game...\n";
    bool exit_game = false;

    while (true) {
        auto& current  = Cuarenta::current_player_state(game);
        auto& opponent = Cuarenta::opposing_player_state(game);

        if (current.score >= 40 || opponent.score >= 40) {
            break;
        }
        if (current.hand.cards.empty() && opponent.hand.cards.empty()) {
            if (game.deck.cards.empty()) {
                Cuarenta::update_captured_cards(game);
                break;
            } else {
                current.hand = game.deck.draw_hand();
                opponent.hand = game.deck.draw_hand();
            }
        }

        bool current_is_human = true;
        if (versus_bot && game.to_move == Cuarenta::Player::P2) {
            current_is_human = false;
        }

        View view = versus_bot ? View::P1Fixed : View::ByTurn;

        if (current_is_human) {
            print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view);
        }

        Cuarenta::MoveList moves { Cuarenta::generate_all_moves(game) };
        size_t chosen{};

        if (current_is_human) {
            while (true) {
                std::cout << "\n\nEnter move (e.g., '5' or '5 = 3 + 2'), "
                             "'help' for commands, or 'quit' to end the game: ";
                std::string input;
                if (!std::getline(std::cin, input)) {
                    exit_game = true;
                    break;
                }
                auto b = input.find_first_not_of(" \t");
                auto e = input.find_last_not_of(" \t");
                if (b != std::string::npos) {
                    input = input.substr(b, e - b + 1);
                } else {
                    continue;
                }

                if (input == "help") {
                    clear_screen();
                    show_help();
                    std::cout << "\nAvailable moves:\n";
                    for (size_t i{}; i < moves.size; i++) {
                        const auto& mv { moves.moves[i] };
                        std::string desc = describe_move_targets(mv);
                        if (!desc.empty()) std::cout << "  " << desc << "\n";
                        else std::cout << "  " << Cuarenta::rank_to_str(mv.get_played_rank()) << "\n";
                    }
                    continue;
                }
                if (input == "quit" || input == "exit") {
                    exit_game = true;
                    break;
                }
                int match { find_move_by_input(input, moves) };
                if (match >= 0) {
                    chosen = static_cast<size_t>(match);
                    break;
                }
                flash_invalid_input(game, view);
                std::cout << "Unrecognised move. Please enter one of the moves listed above, "
                             "'help', or 'quit'.\n";
            }
            if (exit_game) break;
        } else {
            print_game_state(game, Cuarenta::mask_to_vector(game.table.cards), view);
            chosen = choose_move_ai(game, 10);
        }

        if (!exit_game) {
            apply_move_with_animation(game, moves.moves[chosen], view);
            game.advance_turn();
        } else {
            break;
        }
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
        show_banner();
        while (true) {
            std::cout << "> ";
            std::string cmd;
            if (!std::getline(std::cin, cmd)) break;

            auto b = cmd.find_first_not_of(" \t");
            auto e = cmd.find_last_not_of(" \t");
            if (b == std::string::npos) continue;
            cmd = cmd.substr(b, e - b + 1);

            if (cmd == "help") {
                clear_screen();
                show_help();
            } else if (cmd == "play human") {
                run_game(false);
                std::cout << "\nGame finished. Back to main menu.\n";
                sleep_ms(1500);
                clear_screen();
                show_banner();
            } else if (cmd == "play bot") {
                Bot::Bot bot { bot_selection_screen() };
                run_game(bot);
                std::cout << "\nGame finished. Back to main menu.\n";
            } else if (cmd == "quit" || cmd == "exit") {
                std::cout << "Goodbye!\n";
                break;
            } else {
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
