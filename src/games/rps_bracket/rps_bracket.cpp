//
// Created by itssiem on 3/14/24.
//

#include "rps_bracket.h"

#include <bitset>
#include <sstream>

#include "rps_input.h"
#include "rps_player_state.h"

RPSBracket::RPSBracket(Client *host, const std::vector<Client *> &players) : MiniGame(
    host, players, "Rock Paper Scissor Bracket") {
    generate_matches();
    timer.setInterval([&] { Update(1000); }, 1000);
}

void RPSBracket::ProcessInput(Client *client, std::string_view input) {
    std::string in(input);
    RPSInput rps_input(in);

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

    send_playerstates(match);
}

void RPSBracket::Update(int delta_time) {
    for (int i = 0; i < matches.size(); i++) {
        RPSMatch *match = &matches[i];
        // Inform matches with only one player that they are waiting for their opponent
        if ((match->player_1 == nullptr || match->player_2 == nullptr) &&
            !(match->player_1 == nullptr && match->player_2 == nullptr)) {
            if (match->player_1 == nullptr)
                match->player_2->send("Waiting for your opponent...");
            else
                match->player_1->send("Waiting for your opponent...");
            continue;
        }

        // Only advance active matches
        if (match->player_1 != nullptr && match->player_2 != nullptr && match->winner == nullptr) {
            if (match->remaining_time > 0) {
                match->remaining_time -= delta_time;
                send_playerstates(match);
                continue;
            }

            match->Evaluate();
            if (match->winner == nullptr) {
                match->remaining_time = 20000;
            } else {
                update_matches();
            }
            send_playerstates(match);
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
        if (i == starting_matches && players.size() % 2 != 0) {
            // Uneven player count so start one player againt the winner of another match
            matches.emplace_back(nullptr, players[i * 2]);
        }
        else
            matches.emplace_back(nullptr, nullptr);
    }
}

void RPSBracket::update_matches() {
    int starting_matches = players.size() / 2;
    for (int i = 0; i < matches.size(); ++i) {
        RPSMatch *match = &matches[i];
        RPSMatch a = matches[(i - starting_matches) * 2];
        RPSMatch b = matches[(i - starting_matches) * 2 + 1];
        if ((match->player_1 == nullptr || match->player_2 == nullptr) &&
            (a.winner != nullptr || b.winner!= nullptr))
        {
            if (match->player_1 == nullptr)
                match->player_1 = a.winner;
            else
                match->player_2 = b.winner;
        }
    }
}

void RPSBracket::send_playerstates(RPSMatch* match) {
    RPSPlayerState p1(match->remaining_time, match->p1_choice);
    match->player_1->send(p1.Encode(), uWS::BINARY);

    RPSPlayerState p2(match->remaining_time, match->p2_choice);
    match->player_2->send(p2.Encode(), uWS::BINARY);
}
