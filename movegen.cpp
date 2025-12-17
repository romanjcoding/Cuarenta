#include "movegen.h"
#include "rank.h"
#include "cuarenta.h"
#include <vector>

namespace Cuarenta {

std::vector<Move> generate_all_moves(const Table& table, const Hand& hand) {   

    std::vector<Move> moves;
    moves.reserve(MAX_MOVES_PER_TABLE);

    const RankMask low_table_mask = table.cards & LOW_MASK;

    for (const Rank& card : hand.cards) {

        moves.push_back(Move{to_mask(card)});
        const int rank_idx { rank_to_int(card) };

        for (const RankMask addition_pattern : ADDITIONS_BY_RANK[rank_idx]) {
            if ((addition_pattern & low_table_mask) == addition_pattern) {
                moves.emplace_back(Move{addition_pattern | to_mask(card)});
            }
        }
    }
    return moves;
}

}