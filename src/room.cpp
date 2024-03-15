//
// Created by itssiem on 3/13/24.
//

#include "room.h"

#include "client.h"

Room::Room() {
    room_id = -1;
    current_game = nullptr;
}

Room::Room(int id) {
    room_id = id;
    current_game = nullptr;
}

void Room::add_client(Client* client) {
    clients.push_back(client);
}

void Room::remove_client(Client* client) {
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i] == client) {
            clients.erase(clients.begin() + i);
            return;
        }
    }
}

void Room::send(const std::string &payload) {
    for (Client* client: clients) {
        client->ws->send(payload, uWS::TEXT);
    }
}

std::ostream & operator<<(std::ostream &stream, const Room &room) {
    std::string game_name = room.current_game != nullptr ? room.current_game->name : "Nothing";
    stream << room.room_id << ", playing " << game_name << ", " << room.clients.size() << " clients connected";
    return stream;
}

int generate_room_id(const std::map<int, Room> &rooms) {
    const int random = rand();
    int id = random % 10000;
    while (rooms.contains(id)) {
        id++;
    }
    return id;
}
