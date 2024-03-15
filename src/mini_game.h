//
// Created by itssiem on 3/13/24.
//

#ifndef MINIGAME_H
#define MINIGAME_H
#include <string>
#include <vector>

#include "client.h"

class MiniGame {
public:
    std::string name;
    Client* host;
    std::vector<Client*> players;
    MiniGame(Client* host, const std::vector<Client*>& players, const std::string& name);
    virtual void ProcessInput(Client* client, std::string_view input) = 0;
    virtual void Update(int delta_time) = 0;
};

#endif //MINIGAME_H
