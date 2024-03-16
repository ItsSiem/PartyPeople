//
// Created by itssiem on 3/14/24.
//

#ifndef RPS_INPUT_H
#define RPS_INPUT_H
#include "rps_choice.h"
#include "../../game_input.h"

struct RPSInput : public GameInput {
public:
    RPSChoice choice;
    RPSInput(std::string& payload);
    bool Validate() override;
    bool Validate(std::string& payload) override;
    GameInput* Decode(std::string &payload) override;
    std::string Encode() override;
private:
    const uint8_t choice_mask = 0b11;
};

#endif //RPS_INPUT_H
