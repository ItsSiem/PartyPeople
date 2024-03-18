//
// Created by itssiem on 3/18/24.
//

#ifndef RPS_PLAYER_STATE_H
#define RPS_PLAYER_STATE_H
#include "rps_choice.h"
#include "../../game_state.h"

class RPSPlayerState : public GameState {
public:
    int remaining_time;
    RPSChoice selected;
    RPSPlayerState(int remaining_millis, RPSChoice choice);
    std::string Encode() override;
};

struct RPSPlayerStatePayload {
    uint8_t selected : 2;
    uint8_t remaining_time : 6;
};

#endif //RPS_PLAYER_STATE_H
