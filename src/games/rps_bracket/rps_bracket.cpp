//
// Created by itssiem on 3/14/24.
//

#include "rps_bracket.h"

#include <bitset>
#include <sstream>

#include "rps_input.h"

RPSBracket::RPSBracket(Client *host, const std::vector<Client *> &players) : MiniGame(
    host, players, "Rock Paper Scissor Bracket") {
    round_time = 20000;
    generate_matches();
}

void RPSBracket::ProcessInput(Client *client, std::string_view input) {
    std::string i(input);
    RPSInput rps_input(i);

    std::string s;
    switch (rps_input.choice) {
        case ROCK:
            s = "ROCK";
            break;
        case PAPER:
            s = "PAPER";
            break;
        case SCISSOR:
            s = "SCISSOR";
            break;
        case NONE:
            s = "NONE";
            break;
        default:
            s = "UNKNOWN";
            break;
    }

    std::cout << "decoded: " << rps_input.choice << " (" << s << ")" << std::endl;

    RPSMatch* match = nullptr;
    for (int i = 0; i < matches.size(); i++) {
        RPSMatch* m = &matches[i];
        if ((m->player_1 == client || m->player_2 == client) && m->winner == nullptr) {
            match = m;
            break;
        }
    }
    if (match == nullptr) {
        std::cerr << "No match found for this input" << std::endl;
        return;
    }

    if (client == match->player_1)
        match->p1_choice = rps_input.choice;
    else
        match->p2_choice = rps_input.choice;
}

void RPSBracket::Update(int delta_time) {
    if (round_time > 0)
        round_time -= delta_time;
    else {
        for (auto &match: matches) {
            if (match.player_1 != nullptr && match.player_2 != nullptr && match.winner == nullptr) {
                match.Evaluate();
                if (match.winner == nullptr)
                    std::cout << match.player_1->client_id << " vs " << match.player_2->client_id << " : DRAW" <<
                            std::endl;
                else
                    std::cout << match.player_1->client_id << " vs " << match.player_2->client_id << " : " << match.
                            winner->client_id << " won" << std::endl;
            }
        }
    }
}

void RPSBracket::generate_matches() {
    int amount_of_matches = players.size() - 1;
    for (int i = 0; i < players.size() / 2; ++i) {
        // Starting matches
        matches.emplace_back(players[i], players[i + 1]);
    }
    for (int i = players.size() / 2; i < amount_of_matches; ++i) {
        if (i == players.size() && players.size() % 2 != 0) {
            // Uneven player count so start one player againt the winner of another match
            matches.emplace_back(players[i], matches[i - 1].winner);
        }
        matches.emplace_back(matches[i - 1].winner, matches[i - 2].winner);
    }
}
