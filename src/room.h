//
// Created by itssiem on 3/13/24.
//

#ifndef ROOM_H
#define ROOM_H
#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "client.h"
#include "mini_game.h"


class Client;

class Room {
public:
    int room_id;
    std::vector<Client> clients;
    MiniGame current_game;
    Room();
    Room(int id);
    void add_client(const Client& client);
    void remove_client(const Client& client);
};

int generate_room_id(const std::map<int, Room> &rooms);

#endif //ROOM_H
