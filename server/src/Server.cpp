#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <netinet/in.h>
#include "../include/Game.h"
#include "../../common/include/MessageProtocol.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include "Server.h"

#define PORT 8080

Server::Server(int port) : server_port(port), server_fd(-1), running(false) {}
Server::~Server()
{
    stop();
}

void Server::start()
{
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    running = true;
    acceptThread = std::thread(&Server::acceptClients, this);
}

void Server::stop()
{
    running = false;
    if (acceptThread.joinable())
    {
        acceptThread.join();
    }
    if (server_fd != -1)
    {
        close(server_fd);
    }
}

int server_fd;
std::atomic<bool> running;
std::thread acceptThread;

void Server::acceptClients()
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int new_socket;

    while (running)
    {
        std::cout << "Waiting for new connection..." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            continue;
        }

        // Handle client in a separate thread
        std::thread clientThread(&Server::handleClient, this, new_socket);
        clientThread.detach(); // You may want to keep track of these threads or clients
    }
}

void Server::handleClient(int clientSocket)
{
    Tetris::Game game;
    game.startNewGame();
    bool running = true;

    while (running)
    {
        MessageHeader header;
        if (recv(clientSocket, &header, sizeof(header), 0) <= 0)
        {
            running = false;
            break; // Client disconnected or error
        }

        switch (header.messageType)
        {
        case MessageType::ID_RESPONSE:
            handleIDResponse(clientSocket, header);
            break;
        case MessageType::GAME_ACTION:
            handleGameAction(clientSocket, header, game);
            break;
        }

        if (!game.isGameOver())
        {
            game.updateGame();
            sendGameState(clientSocket, game);
        }
        else
        {
            sendGameOver(clientSocket);
            running = false;
        }
    }

    close(clientSocket);
}

void Server::handleIDResponse(int clientSocket, const MessageHeader &header)
{
    // Process client ID (you can store it, log it, etc.)
    std::cout << "Received ID response from client " << header.senderID << std::endl;
}

void Server::handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game)
{
    // Process game action from client
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, header.messageLength, 0);
    if (bytesRead > 0)
    {
        std::string action(buffer, bytesRead);
        // Interpret and apply action to the game
        if (action == "MOVE_LEFT")
            game.moveCurrentPieceLeft();
        if (action == "MOVE_RIGHT")
            game.moveCurrentPieceRight();
        if (action == "ROTATE")
            game.rotateCurrentPiece();
        if (action == "MOVE_DOWN")
            game.moveCurrentPieceDown();
        // Add other actions as needed
    }
}

void Server::sendGameState(int clientSocket, const Tetris::Game &game)
{
    std::string gameState = game.getCurrentState(); // Assuming such a method exists
    MessageHeader header;
    header.messageType = MessageType::GAME_STATE;
    header.messageLength = gameState.size();
    header.senderID = 0;   // Server ID
    header.receiverID = 1; // Client ID

    send(clientSocket, &header, sizeof(header), 0);
    send(clientSocket, gameState.c_str(), gameState.size(), 0);
}

void Server::sendGameOver(int clientSocket)
{
    std::string message = "Game Over";
    MessageHeader header;
    header.messageType = MessageType::GAME_OVER;
    header.messageLength = message.size();
    header.senderID = 0;   // Server ID
    header.receiverID = 1; // Client ID

    send(clientSocket, &header, sizeof(header), 0);
    send(clientSocket, message.c_str(), message.size(), 0);
}
;
