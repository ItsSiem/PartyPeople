#include <iostream>
#include <sstream>

#include "client.h"
#include "room.h"
#include "uWebSockets/App.h"

std::map<int, Room> rooms;
std::map<int, Client> clients;

int main() {
    srand(time(nullptr));
    uWS::SSLApp()
            .get("/host", [](auto *res, auto *req) {
                Room room(generate_room_id(rooms));
                rooms[room.room_id] = room;

                std::ostringstream ostr;
                ostr << "Room id: " << room.room_id << std::endl;

                res->end(ostr.str());
            })
            .get("/rooms", [](auto *res, auto *req) {
                for (auto room: rooms) {
                    std::ostringstream ostr;
                    ostr << "Room " << room.second.room_id << " " << room.second.clients.size() <<
                            " clients connected" << std::endl;
                    res->write(ostr.str());
                }
                res->end();
            })
            .get("/:room/ping", [](auto *res, auto *req) {
                int room_id = std::stoi(std::string(req->getParameter("room")));
                if (!rooms.contains(room_id)) {
                    res->writeStatus("400");
                    res->write("Invalid room code");
                    res->end();
                }

                rooms[room_id].send("Ping");
                res->end();
            })
            .get("/clients", [](auto *res, auto *req) {
                for (auto client: clients) {
                    std::ostringstream ostr;
                    ostr << "Client " << client.second.client_id << " connected to room " << client.second.room_id <<
                            std::endl;
                    res->write(ostr.str());
                }
                res->end();
            })
            .ws<SocketData>("/join/:room", {
                                /* Settings */
                                .compression = uWS::CompressOptions::DISABLED,
                                .maxPayloadLength = 100 * 1024 * 1024, // TODO: figure out what a good value is
                                .idleTimeout = 16, // TODO: figure out what a good value is
                                .maxBackpressure = 100 * 1024 * 1024, // TODO: figure out what a good value is
                                .closeOnBackpressureLimit = false, // TODO: figure out what a good value is
                                .resetIdleTimeoutOnSend = false, // TODO: figure out what a good value is
                                .sendPingsAutomatically = true,
                                /* Handlers */
                                .upgrade = [](auto *res, auto *req, auto *context) {
                                    /* You may read from req only here, and COPY whatever you need into your PerSocketData.
                                     * PerSocketData is valid from .open to .close event, accessed with ws->getUserData().
                                     * HttpRequest (req) is ONLY valid in this very callback, so any data you will need later
                                     * has to be COPIED into PerSocketData here. */

                                    int room_id = std::stoi(std::string(req->getParameter("room")));

                                    if (!rooms.contains(room_id)) {
                                        res->writeStatus("400");
                                        res->write("Invalid room code");
                                        res->end();
                                    }

                                    /* Immediately upgrading without doing anything "async" before, is simple */
                                    res->template upgrade<SocketData>({
                                                                          .room_id = room_id
                                                                      },
                                                                      req->getHeader("sec-websocket-key"),
                                                                      req->getHeader("sec-websocket-protocol"),
                                                                      req->getHeader("sec-websocket-extensions"),
                                                                      context);

                                    /* If you don't want to upgrade you can instead respond with custom HTTP here,
                                     * such as res->writeStatus(...)->writeHeader(...)->end(...); or similar.*/

                                    /* Performing async upgrade, such as checking with a database is a little more complex;
                                     * see UpgradeAsync example instead. */
                                },
                                .open = [](auto *ws) {
                                    Client client(generate_client_id(clients), ws->getUserData()->room_id, false, ws);
                                    clients[client.client_id] = client;
                                    rooms[client.room_id].add_client(client);
                                    ws->getUserData()->client_id = client.client_id;

                                    std::ostringstream ostr;
                                    ostr << "client id: " << ws->getUserData()->client_id << std::endl << "room id: " <<
                                            ws->getUserData()->room_id << std::endl;

                                    ws->send(ostr.str(), uWS::TEXT);
                                },
                                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                                },
                                .dropped = [](auto */*ws*/, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
                                    /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
                                },
                                .drain = [](auto */*ws*/) {
                                    /* Check ws->getBufferedAmount() here */
                                },
                                .ping = [](auto */*ws*/, std::string_view) {
                                    /* Not implemented yet */
                                },
                                .pong = [](auto */*ws*/, std::string_view) {
                                    /* Not implemented yet */
                                },
                                .close = [](auto *ws, int /*code*/, std::string_view /*message*/) {
                                    // Remove from clients map
                                    Client client = clients[ws->getUserData()->client_id];
                                    clients.erase(client.client_id);
                                    rooms[client.room_id].remove_client(client);
                                }
                            })
            .listen(3001, [](auto *listen_socket) {
                if (listen_socket) {
                    std::cout << "Listening on port " << 3001 << std::endl;
                }
            }).run();

    std::cout << "Failed to listen on port 3001" << std::endl;
}
