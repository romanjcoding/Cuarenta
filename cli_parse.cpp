#include "cli_parse.h"

#include "rank.h"
#include "cuarenta.h"

#include <optional>
#include <string>

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
    for (size_t i{1}; i <= Cuarenta::NUM_RANKS; i++) {
        Cuarenta::Rank r { Cuarenta::int_to_rank(static_cast<int>(i)) };
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
                if (value == static_cast<int>(i)) {
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
    std::ranges::transform(input, input.begin(), [](unsigned char c){ return std::tolower(c); });


    if (input == "help") { return InputData { .help = true }; }
    if (input == "quit") { return InputData { .quit = true }; }

    if (input == "play bot")   { return InputData { .play_bot = true }; }
    if (input == "play human") { return InputData { .play_human = true }; }

    if (input == "play child") { return InputData { .bot = Bot::BOT_CHILD }; }
    if (input == "play robot") { return InputData { .bot = Bot::BOT_ROBOT }; }
    if (input == "play man")   { return InputData { .bot = Bot::BOT_MAN   }; }
    if (input == "play cheat") { return InputData { .bot = Bot::BOT_CHEAT }; }

    std::string lhs;
    std::string rhs;
    size_t eq_pos = input.find('=');
    if (eq_pos != std::string::npos) {
        lhs = input.substr(0, eq_pos);
        rhs = input.substr(eq_pos + 1);
    } else {
        lhs = input;
    }

    auto opt { try_parse_rank_token(lhs) };
    if (!opt.has_value()) { return InputData { .err = true }; }
    auto played_rank = opt.value();

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
                if (!finish_token(token)) { return InputData { .err = true }; }
                token.clear();
            } else {
                token += ch;
            }
        }
        if (!token.empty()) {
            if (!finish_token(token)) { return InputData { .err = true }; }
        }
        for (auto r : target_ranks) {
            targets_mask = targets_mask | Cuarenta::to_mask(r);
        }
    }
    
    return InputData { .move = Cuarenta::Move { targets_mask | to_mask(played_rank) } };
}

} // namespace cli