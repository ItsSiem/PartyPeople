//
// Created by itssiem on 3/18/24.
//

#include"rps_player_state.h"

#include <bit>
#include <bitset>
#include <iostream>

RPSPlayerState::RPSPlayerState(int remaining_millis, RPSChoice choice) {
    remaining_time = remaining_millis / 1000 + 1;
    selected = choice;
}

std::string RPSPlayerState::Encode() {
    RPSPlayerStatePayload payload {.selected = (uint8_t)selected, .remaining_time = (uint8_t)remaining_time};
    uint8_t p[2];
    p[0] = std::bit_cast<uint8_t>(payload);;
    p[1] = 0x00;
    std::string s((char *)p);
    return s;
}


