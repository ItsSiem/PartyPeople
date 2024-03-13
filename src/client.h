//
// Created by itssiem on 3/13/24.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <uWebSockets/WebSocket.h>

#include "SocketData.h"

class Client {
public:
    int client_id;
    int room_id;
    bool is_host;
    uWS::WebSocket<true, true, SocketData>* ws;
    Client();
    Client(int client, int room, bool host, uWS::WebSocket<true, true, SocketData>* ws);
};
bool operator==(const Client& a, const Client& b);

int generate_client_id(const std::map<int, Client>& clients);

#endif //CLIENT_H
