#include "movegen.h"
#include "dynamic_array.h"
#include "rank.h"
#include "cuarenta.h"

#include <vector>
#include <cassert>

namespace Cuarenta {

util::dynamic_array<RankMask, MAX_MOVES_PER_TABLE> generate_all_moves(const Game_State& game) {   

    util::dynamic_array<RankMask, MAX_MOVES_PER_TABLE> available_moves{};

    const RankMask low_table_mask { game.table.cards & LOW_MASK };

    for (const Rank& card : current_player_state(game).hand.cards) {

        assert(available_moves.size() < MAX_MOVES_PER_TABLE);
        available_moves.push_back(to_mask(card));

        const int rank_idx { rank_to_int(card) };

        // todo: make better?
        for (const RankMask addition_pattern : ADDITIONS_BY_RANK[rank_idx]) {
            if ((addition_pattern & low_table_mask) == addition_pattern) {
                RankMask move { addition_pattern | to_mask(card) };
                if (!available_moves.contains(move)) {
                    assert(available_moves.size() < MAX_MOVES_PER_TABLE);
                    available_moves.push_back(move);
                }
            }
        }
    }
    return available_moves;
}

}