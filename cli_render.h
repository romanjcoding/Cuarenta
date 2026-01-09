#pragma once
#include "cli.h"
#include <cstdint>
#include <string>
#include <string_view>

namespace cli {

constexpr void clear_screen() {
    #if defined(_WIN32)
        std::system("cls");
    #else
        std::cout << "\033[2J\033[H" << std::flush;
    #endif
}

struct RenderOpts {
    std::vector<size_t> table_idx_to_highlight { };
    std::string_view table_color      { ansi::reset };

    std::optional<size_t> bottom_hi_index {};
    std::string_view bottom_color    { ansi::reset };

    std::optional<size_t> top_hi_index {};
    std::string_view top_color       { ansi::reset };

    bool error_flash { false };

    std::string banner {};
    std::string_view banner_color    { ansi::magenta };
    bool banner_bold { true };
};

void print_menu_banner();
void print_help_banner();
void print_bot_select_banner();


static constexpr int TABLE_CAPACITY { 11 }; // 10 (max capacity) + 1 (for intermediate captures)
static constexpr int CARD_CELL_W    { 8 }; 
static constexpr int TABLE_INNER_W  { TABLE_CAPACITY * CARD_CELL_W };
static constexpr int CARD_LINES_H   { 5 };

void print_cards_box(const std::vector<Cuarenta::Rank>& cards,
                     bool is_empty,
                     std::optional<size_t> highlight_index = std::nullopt,
                     Cuarenta::RankMask highlight_mask = Cuarenta::to_mask(0),
                     std::string_view highlight_color  = ansi::reset,
                     bool error_flash                  = false);

std::vector<std::string> build_cards_box_lines(
    const std::vector<Cuarenta::Rank>& cards,
    bool is_empty,
    std::optional<size_t> highlight_index,
    std::vector<size_t> table_idx_to_highlight,
    std::string_view highlight_color,
    bool error_flash);

void print_table_framed(const std::vector<Cuarenta::Rank>& table_cards,
                        const RenderOpts& opt);

void print_game_state(const Cuarenta::Game_State& game,
                      const std::vector<Cuarenta::Rank>& visible_table,
                      View view = View::ByTurn,
                      const RenderOpts& opt = {});

void print_scoreboard(const Cuarenta::Game_State& game);
void flash_invalid_input(const Cuarenta::Game_State& game, View view);


} // namespace cli