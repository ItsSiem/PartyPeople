//
// Created by itssiem on 3/14/24.
//

#ifndef RPS_INPUT_H
#define RPS_INPUT_H
#include "rps_choice.h"

struct RPSInput {
    RPSChoice choice;
};

bool validate(RPSInput);

#endif //RPS_INPUT_H
