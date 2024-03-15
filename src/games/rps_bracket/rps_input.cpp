//
// Created by itssiem on 3/15/24.
//

#include "rps_input.h"

bool validate(RPSInput input) {
    return input.choice >= 0 && input.choice <= 3;
}
