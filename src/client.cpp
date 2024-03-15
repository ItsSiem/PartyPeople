//
// Created by itssiem on 3/13/24.
//

#include "client.h"

#include "room.h"

Client::Client() {
    client_id = -1;
    name = "-1";
    room = nullptr;
    is_host = false;
    ws = nullptr;
}

Client::Client(int client, Room* room, bool host, uWS::WebSocket<true, true, SocketData>* ws) {
    client_id = client;
    name = std::to_string(client_id);
    this->room = room;
    is_host = host;
    this->ws = ws;
}

bool operator==(const Client &a, const Client &b) {
    return a.client_id == b.client_id;
}

std::ostream & operator<<(std::ostream& stream, const Client &client) {
    stream << client.name << "(i" << client.client_id << "r" << client.room->room_id << ")";
    return stream;
}


int generate_client_id(const std::map<int, Client>& clients) {
    int id = 0;
    while (clients.contains(id))
        id++;
    return id;
}

