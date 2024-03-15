//
// Created by itssiem on 3/14/24.
//

#include "mini_game.h"

MiniGame::MiniGame(Client *host, const std::vector<Client *>& players, const std::string& name) {
    this->host = host;
    this->players = players;
    this->name = name;
}

