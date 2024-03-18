//
// Created by itssiem on 3/14/24.
//

#include "rps_bracket.h"

#include <bitset>
#include <sstream>

#include "rps_input.h"

RPSBracket::RPSBracket(Client *host, const std::vector<Client *> &players) : MiniGame(
    host, players, "Rock Paper Scissor Bracket") {
    generate_matches();
    int tick_time = 16; //ms
    timer.setInterval([&] { Update(16); }, 16);
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

    RPSMatch *match = nullptr;
    for (int i = 0; i < matches.size(); i++) {
        RPSMatch *m = &matches[i];
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
    for (auto &match: matches) {
        // Inform matches with only one player that they are waiting for their opponent
        if ((match.player_1 == nullptr || match.player_2 == nullptr) && !(
                match.player_1 == nullptr && match.player_2 == nullptr)) {
            if (match.player_1 == nullptr)
                match.player_2->send("Waiting for your opponent...");
            else
                match.player_1->send("Waiting for your opponent...");
            continue;
        }

        // Only advance active matches
        if (match.player_1 != nullptr && match.player_2 != nullptr && match.winner == nullptr) {
            if (match.remaining_time > 0) {
                match.remaining_time -= delta_time;
                continue;
            }

            match.Evaluate();
            std::cout << *match.player_1 << " vs " << *match.player_2 << " : ";
            if (match.winner == nullptr) {
                std::cout << "DRAW" << std::endl;
                match.player_1->send("Draw, try again");
                match.player_2->send("Draw, try again");
                match.remaining_time = 20000;
            } else {
                std::cout << *match.winner << " won" << std::endl;
                match.winner->send("You win!");
            }
        }
    }
    for (RPSMatch match: matches) {
        if (match.winner == nullptr)
            return;
    }
    std::cout << "All matches finished, the winner is " << *matches.back().winner << std::endl;
    matches.back().winner->send("You won the tournament!!");
    timer.stop();
}

void RPSBracket::generate_matches() {
    int amount_of_matches = players.size() - 1;
    int starting_matches = players.size() / 2;

    if (amount_of_matches == 0) {
        throw std::invalid_argument("Not enough players to start a game");
    }

    for (int i = 0; i < starting_matches; ++i) {
        // Starting matches
        matches.emplace_back(players[i * 2], players[i * 2 + 1]);
    }
    for (int i = starting_matches; i < amount_of_matches; ++i) {
        if (i == players.size() && players.size() % 2 != 0) {
            // Uneven player count so start one player againt the winner of another match
            matches.emplace_back(players[i * 2], matches[i * 2 - 1].winner);
        }
        matches.emplace_back(matches[i - 1].winner, matches[i - 2].winner);
    }
}
