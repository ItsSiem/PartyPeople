//
// Created by itssiem on 3/15/24.
//

#include "rps_input.h"

#include <bitset>
#include <iostream>
#include <sstream>


RPSInput::RPSInput(std::string &payload) {
    Decode(payload);
}

GameInput* RPSInput::Decode(std::string &payload) {
    if (!this->Validate(payload)) {
        std::cerr << "Could not create RPSInput, payload malformed!" << std::endl;
        throw std::invalid_argument("Payload malformed");
    }
    uint8_t* p = reinterpret_cast<uint8_t *>(&payload[0]);
    choice = (RPSChoice)(*p & choice_mask);
    return this;
}

std::string RPSInput::Encode() {
    uint8_t payload = 0x00;
    std::cout << std::bitset<8>(payload) << std::endl;
    payload = payload | (uint8_t)choice;
    std::cout << std::bitset<8>(payload) << std::endl;
    std::string s(payload, sizeof(payload));
    return s;
}

bool RPSInput::Validate() {
    return choice >= 0 && choice <= 3;
}

bool RPSInput::Validate(std::string& payload) {
    uint8_t* p = reinterpret_cast<uint8_t *>(&payload[0]);
    return *p >= 0 && *p <= 3;
}
