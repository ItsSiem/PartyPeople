//
// Created by itssiem on 3/15/24.
//

#ifndef GAME_INPUT_H
#define GAME_INPUT_H
#include <string>

class GameInput {
public:
    virtual bool Validate() = 0;
    virtual GameInput* Decode(std::string& payload) = 0;
    virtual std::string Encode() = 0;
};

#endif //GAME_INPUT_H
