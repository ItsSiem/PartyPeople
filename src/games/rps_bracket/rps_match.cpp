//
// Created by itssiem on 3/14/24.
//

#include "rps_match.h"

RPSMatch::RPSMatch(Client *p1, Client *p2) {
    player_1 = p1;
    player_2 = p2;
    std::cout << "match constructor" << std::endl;
    p1_choice = NONE;
    p2_choice = NONE;
    winner = nullptr;
}

void RPSMatch::Evaluate() {
    if (p1_choice == p2_choice)
        return;

    if ((p1_choice == ROCK && p2_choice == SCISSOR) ||
        (p1_choice == PAPER && p2_choice == ROCK) ||
        (p1_choice == SCISSOR && p2_choice == PAPER))
        winner = player_1;
    else
        winner = player_2;
}
