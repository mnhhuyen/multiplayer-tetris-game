#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include "Game.h"
#include "../../common/include/MessageProtocol.hpp"

class Server
{
private:
    void acceptClients();
    void handleClient(int clientSocket, unsigned long long clientID);
    void startGameForClient(unsigned long long clientID);
    void sendMessage(int clientSocket, const MessageHeader &header, const std::string &payload);
    std::string receiveMessage(int clientSocket, MessageHeader &header);
    void handleIDResponse(int clientSocket, const MessageHeader &header, unsigned long long client_id);
    void handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game, unsigned long long client_id);
    void sendGameState(int clientSocket, const Tetris::Game &game);
    void sendGameOver(int clientSocket);
    void updateGameForClient(unsigned long long client_id);
    void sendMessageToClient(unsigned long long client_id, MessageType messageType, const std::string& payload);

    int server_port;
    int server_fd;
    std::atomic<bool> running;
    std::thread acceptThread;

    std::mutex clientSocketsMutex;
    std::unordered_map<unsigned long long, int> clientSockets;

    std::mutex clientGamesMutex;
    std::unordered_map<unsigned long long, Tetris::Game> clientGames;

    std::mutex client_thread_mutex;
    std::vector<std::thread> client_threads;

    unsigned long long next_client_id = 0;

public:
    Server(int port);
    ~Server();
    void start();
    void stop();
    // void acceptClients();
    // std::string receiveMessage(int clientSocket, MessageHeader &header);
    // void sendMessage(int clientSocket, const MessageHeader &header, const std::string &payload);
    // void startGameForClient(unsigned long long client_id);
    // void handleClient(int clientSocket, unsigned long long client_id);
    // void handleIDResponse(int clientSocket, const MessageHeader &header, unsigned long long client_id);
    // void handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game, unsigned long long client_id);
    // void sendGameState(int clientSocket, const Tetris::Game &game);
    // void sendGameOver(int clientSocket);
};
