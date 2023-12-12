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
    void stop();

private:
    int server_socket;
    int server_port;
    int server_fd;
    std::vector<std::thread> client_threads;
    std::atomic<bool> running;
    std::thread acceptThread;
    void acceptClients();
    void handleClient(int client_socket);
    void handleIDResponse(int clientSocket, const MessageHeader &header);
    void handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game);
    void sendGameState(int clientSocket, const Tetris::Game &game);
    void sendGameOver(int clientSocket);
};
