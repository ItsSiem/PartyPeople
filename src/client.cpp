//
// Created by itssiem on 3/13/24.
//

#include "client.h"
Client::Client() {
    client_id = -1;
    room_id = -1;
    is_host = false;
    ws = nullptr;
}

Client::Client(int client, int room, bool host, uWS::WebSocket<true, true, SocketData>* ws) {
    client_id = client;
    room_id = room;
    is_host = host;
    this->ws = ws;
}

bool operator==(const Client &a, const Client &b) {
    return a.client_id == b.client_id;
}


int generate_client_id(const std::map<int, Client>& clients) {
    int id = 0;
    while (clients.contains(id))
        id++;
    return id;
}

