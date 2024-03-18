#include <iostream>
#include <sstream>

#include "SocketData.h"
#include "room.h"
#include "client.h"
#include "games/rps_bracket/rps_bracket.h"
#include "uWebSockets/App.h"

std::map<int, Room> rooms;
std::map<int, Client> clients;

void create_room(uWS::WebSocket<true, true, SocketData>*& ws) {
    int room_id = generate_room_id(rooms);
    rooms[room_id] = Room(room_id);
    ws->getUserData()->room = &rooms[room_id];
}

void create_client(const bool host, uWS::WebSocket<true, true, SocketData>*& ws) {
    int client_id = generate_client_id(clients);
    clients[client_id] = Client(generate_client_id(clients), ws->getUserData()->room, host, ws);
    ws->getUserData()->client = &clients[client_id];
    rooms[ws->getUserData()->room->room_id].add_client(&clients[client_id]);
}

void remove_client(uWS::WebSocket<true, true, SocketData>*& ws) {
    // Remove client from map
    Client* client = ws->getUserData()->client;
    client->room->remove_client(client);

    if (client->is_host) {
        // Todo: Promote new client to host?
    }

    // Close room if empty
    if (client->room->clients.empty())
        rooms.erase(client->room->room_id);

    clients.erase(client->client_id);
}

void send_join_message(uWS::WebSocket<true, true, SocketData>*& ws) {
    std::ostringstream ostr;
    ostr << "client  " << ws->getUserData()->client->name << " joined room " <<
            ws->getUserData()->room->room_id << std::endl;

    ws->getUserData()->room->send(ostr.str());
}

void process_message(uWS::WebSocket<true, true, SocketData>*& ws, std::string_view message, uWS::OpCode opCode) {
    Room* room = ws->getUserData()->room;
    Client* client = ws->getUserData()->client;

    if (message == "rps create") {
        room->current_game = new RPSBracket(client, room->clients); // Todo: So uh we gotta clean this memory up at some point
        ws->send("Created RPS", uWS::TEXT);
        return;
    }
    if (message == "rps shoot") {
        room->current_game->Update(20000);
        return;
    }

    try {
        room->current_game->ProcessInput(client, message);
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        ws->send(e.what(), uWS::TEXT);
    }
}

int main() {
    srand(time(nullptr));
    uWS::SSLApp()
            .get("/rooms", [](auto *res, auto *req) {
                for (const auto& room: rooms) {
                    std::ostringstream ostr;
                    ostr << room.second << std::endl;
                    res->write(ostr.str());
                }
                res->end();
            })
            .get("/clients", [](auto *res, auto *req) {
                for (const auto& client: clients) {
                    std::ostringstream ostr;
                    ostr << client.second << std::endl;
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
                                                                          .room = &rooms[room_id]
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
                                    process_message(ws, message, opCode);
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
                                    // ws->getUserData()->room->current_game = new RPSBracket(ws->getUserData()->client, ws->getUserData()->room->clients); // Todo: So uh we gotta clean this memory up at some point
                                    // ws->send("Created RPS", uWS::TEXT);
                                },
                                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                                    process_message(ws, message, opCode);
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
