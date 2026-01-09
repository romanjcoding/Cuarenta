#include "cli_render.h"
#include "cli.h"
#include "ansi.h"
#include "rank.h"
#include "game_state.h"
#include "cuarenta.h"

#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <iomanip>

namespace cli {

void print_menu_banner() {
    std::cout << "============================================================\n";
    std::cout << "                  Welcome to CUARENTA CLI                   \n";
    std::cout << "============================================================\n";
    std::cout << "Cuarenta is a traditional fishing card game from Ecuador.\n";
    std::cout << "Reach 40 points before your opponent to win.\n";
    std::cout << "Type 'help' to see available commands.\n\n";
}

void print_help_banner() {
    std::cout << "============================================================\n";
    std::cout << "                  Welcome to the Help Menu                  \n";
    std::cout << "============================================================\n";
    std::cout << "\nAvailable commands:\n";
    std::cout << "  play human   : start a two–player (human vs human) game\n";
    std::cout << "  play bot     : start a game against the computer\n";
    std::cout << "  help         : display this help message\n";
    std::cout << "  quit         : exit the program\n\n";
}

void print_bot_select_banner() {
    std::cout << "============================================================\n";
    std::cout << "                    Bot Selection Screen                    \n";
    std::cout << "============================================================\n";
    std::cout << "\n Choose a bot to play against:\n";
    std::cout << "   'play child'\n";
    std::cout << "   'play robot'\n";
    std::cout << "   'play man'\n";
    std::cout << "   'play cheater'\n";
}

void print_cards_box(const std::vector<Cuarenta::Rank>& cards,
                     bool is_empty,
                     std::optional<size_t> highlight_index,
                     Cuarenta::RankMask highlight_mask,
                     std::string_view highlight_color,
                     bool error_flash) {

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

void print_game_state(const Cuarenta::Game_State& game,
                      const std::vector<Cuarenta::Rank>& visible_table,
                      View view,
                      const RenderOpts& opt) {
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

} // namespace cli