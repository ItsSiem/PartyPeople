//
// Created by itssiem on 3/13/24.
//

#include "room.h"

#include "client.h"

Room::Room() {
    room_id = -1;
}

Room::Room(int id) {
    room_id = id;
}

void Room::add_client(const Client &client) {
    clients.push_back(client);
}

void Room::remove_client(const Client &client) {
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i] == client) {
            clients.erase(clients.begin() + i);
            return;
        }
    }
}

int generate_room_id(const std::map<int, Room> &rooms) {
    const int random = rand();
    int id = random % 10000;
    while (rooms.contains(id)) {
        id++;
    }
    return id;
}
