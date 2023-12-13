#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "../include/Game.h"
#include "../../common/include/MessageProtocol.hpp"

class Server {
public:
    Server(int port);
    ~Server();
    void start();
    void stop();

private:
    int server_fd;
    int server_port;
    std::vector<std::thread> client_threads;
    std::atomic<bool> running;
    std::thread acceptThread;
    std::mutex client_thread_mutex;

    unsigned long long next_client_id = 0;
    std::mutex client_id_mutex;

    void acceptClients();
    void handleClient(int client_socket, unsigned long long client_id);
    void handleIDResponse(int clientSocket, const MessageHeader &header, unsigned long long client_id);
    void handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game, unsigned long long client_id);
    void sendGameState(int clientSocket, const Tetris::Game &game);
    void sendGameOver(int clientSocket);
};
