//
// Created by itssiem on 3/14/24.
//

#ifndef RPS_MATCH_H
#define RPS_MATCH_H
#include "rps_choice.h"
#include "../../client.h"

class RPSMatch {
public:
    Client* player_1;
    RPSChoice p1_choice;
    Client* player_2;
    RPSChoice p2_choice;
    Client* winner;
    int remaining_time;
    RPSMatch(Client* p1, Client* p2, int round_time = 20000);
    void Evaluate();
};

#endif //RPS_MATCH_H
