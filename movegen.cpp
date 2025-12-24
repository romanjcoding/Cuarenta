#include "movegen.h"
#include "rank.h"
#include "cuarenta.h"
#include <vector>

namespace Cuarenta {

MoveList generate_all_moves(const Game_State& game) {   

    MoveList available_moves{};

    const RankMask low_table_mask { game.table.cards & LOW_MASK };

    for (const Rank& card : current_player_state(game).hand.cards) {

        if (available_moves.size >= MAX_MOVES_PER_TABLE) break; // not needed?
        available_moves.push_back(Move{to_mask(card)});

        const int rank_idx { rank_to_int(card) };

        for (const RankMask addition_pattern : ADDITIONS_BY_RANK[rank_idx]) {
            if ((addition_pattern & low_table_mask) == addition_pattern) {
                if (available_moves.size >= MAX_MOVES_PER_TABLE) break; // not needed?
                available_moves.push_back(Move{addition_pattern | to_mask(card)});
            }
        }
    }
    return available_moves;
}

}