#include <iostream>
#include <sstream>

#include "client.h"
#include "room.h"
#include "uWebSockets/App.h"

std::map<int, Room> rooms;
std::map<int, Client> clients;

void create_room(uWS::WebSocket<true, true, SocketData>*& ws) {
    Room room(generate_room_id(rooms));
    rooms[room.room_id] = room;
    ws->getUserData()->room_id = room.room_id;
}

void create_client(const bool host, uWS::WebSocket<true, true, SocketData>*& ws) {
    Client client(generate_client_id(clients), ws->getUserData()->room_id, host, ws);
    clients[client.client_id] = client;
    ws->getUserData()->client_id = client.client_id;
    rooms[ws->getUserData()->room_id].add_client(client);
}

void remove_client(uWS::WebSocket<true, true, SocketData>*& ws) {
    // Remove client from map
    Client client = clients[ws->getUserData()->client_id];
    clients.erase(client.client_id);
    rooms[client.room_id].remove_client(client);

    if (client.is_host) {
        // Todo: Promote new client to host?
    }

    // Close room if empty
    if (rooms[client.room_id].clients.empty())
        rooms.erase(client.room_id);
}

void send_join_message(uWS::WebSocket<true, true, SocketData>*& ws) {
    std::ostringstream ostr;
    ostr << "client  " << ws->getUserData()->client_id << " joined room " <<
            ws->getUserData()->room_id << std::endl;

    rooms[ws->getUserData()->room_id].send(ostr.str());
}

int main() {
    srand(time(nullptr));
    uWS::SSLApp()
            .get("/rooms", [](auto *res, auto *req) {
                for (const auto& room: rooms) {
                    std::ostringstream ostr;
                    ostr << "Room " << room.second.room_id << " " << room.second.clients.size() <<
                            " clients connected" << std::endl;
                    res->write(ostr.str());
                }
                res->end();
            })
            .get("/clients", [](auto *res, auto *req) {
                for (const auto& client: clients) {
                    std::ostringstream ostr;
                    ostr << "Client " << client.second.client_id << " connected to room " << client.second.room_id <<
                            std::endl;
                    res->write(ostr.str());
                }
                res->end();
            })
            .ws<SocketData>("/join/:room", {
                                /* Handlers */
                                .upgrade = [](auto *res, auto *req, auto *context) {
                                    int room_id = std::stoi(std::string(req->getParameter("room")));

                                    if (!rooms.contains(room_id)) {
                                        res->writeStatus("400");
                                        res->write("Invalid room code");
                                        res->end();
                                        return;
                                    }

                                    res->template upgrade<SocketData>({
                                                                          .room_id = room_id
                                                                      },
                                                                      req->getHeader("sec-websocket-key"),
                                                                      req->getHeader("sec-websocket-protocol"),
                                                                      req->getHeader("sec-websocket-extensions"),
                                                                      context);
                                },
                                .open = [](auto *ws) {
                                    create_client(false, ws);
                                    send_join_message(ws);
                                },
                                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                                },
                                .close = [](auto *ws, int /*code*/, std::string_view /*message*/) {
                                    remove_client(ws);
                                }
                            })
            .ws<SocketData>("/host", {
                                /* Handlers */
                                .open = [](auto *ws) {
                                    create_room(ws);
                                    create_client(true, ws);
                                    send_join_message(ws);
                                },
                                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                                },
                                .close = [](auto *ws, int /*code*/, std::string_view /*message*/) {
                                    remove_client(ws);
                                }
                            })
            .listen(3001, [](auto *listen_socket) {
                if (listen_socket) {
                    std::cout << "Listening on port " << 3001 << std::endl;
                }
            }).run();

    std::cout << "Failed to listen on port 3001" << std::endl;
}
