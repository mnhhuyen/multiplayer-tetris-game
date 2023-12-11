// server/include/Server.h
#pragma once
#include <string>
#include <vector>
#include <thread>
#include "../include/Game.h"
#include "../../common/include/MessageProtocol.hpp"

class Server {
public:
    Server(int port);
    ~Server();
    void start();

private:
    int server_socket;
    int server_port;
    std::vector<std::thread> client_threads;

    void acceptClients();
    void handleClient(int client_socket);
    void handleIDResponse(int clientSocket, const MessageHeader &header);
    void handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game);
    void sendGameState(int clientSocket, const Tetris::Game &game);
    void sendGameOver(int clientSocket);
};
