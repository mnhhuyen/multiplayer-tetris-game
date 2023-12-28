#include "Server.h"
#include "MessageProtocol.hpp"
#include "Game.h"
#include <iostream>
#include <string>
#include <unistd.h> // For close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <mutex>
#include <unordered_map>

#define PORT 8080
#define BACKLOG 10

Server::Server(int port) : server_port(port), server_fd(-1), running(false)
{
    // Initialize server socket and set up the server here
}

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
        perror("ERROR on binding");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0)
    {
        perror("ERROR on listening");
        close(server_fd);
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

    std::lock_guard<std::mutex> guard(client_thread_mutex);
    for (auto &thread : client_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    if (server_fd != -1)
    {
        close(server_fd);
    }
}

void Server::acceptClients()
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while (running)
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("accept");
            continue;
        }

        unsigned long long client_id = next_client_id++;
        printf("got client with id %lf\n", client_id);
        std::lock_guard<std::mutex> guard(client_thread_mutex);
        client_threads.emplace_back([this, new_socket, client_id]()
                                    { this->handleClient(new_socket, client_id); });
    }
}

void Server::handleClient(int clientSocket, unsigned long long client_id)
{
    {
        std::lock_guard<std::mutex> lock(clientSocketsMutex);
        clientSockets[client_id] = clientSocket;
    }

    {
        std::lock_guard<std::mutex> lock(clientGamesMutex);
        Tetris::Game newGame;                        
        clientGames[client_id] = std::move(newGame); 
    }

    bool clientConnected = true;
    while (clientConnected)
    {
        MessageHeader header;
        std::string clientMessage = receiveMessage(clientSocket, header);
        printf("got a message from client\n");
        std::cout << "client message:" << clientMessage << std::endl;
        if (clientMessage.empty())
        {
            clientConnected = false;
            break;
        }

        printf("check1\n");

        auto gameIter = clientGames.find(client_id);
        if (gameIter == clientGames.end())
        {
            // No game found for this client, handle this case appropriately
            continue;
        }
        printf("check2\n");
        Tetris::Game &game = gameIter->second;

        switch (header.messageType)
        {
        case MessageType::GAME_ACTION:
            std::cout << "got GAME_ACTION message type" << std::endl;
            if (clientMessage == "START")
            {
                printf("server start the game for client\n");
                startGameForClient(client_id);
            }
            else if (clientMessage == "move_left")
            {
                game.moveCurrentPieceLeft();
                updateGameForClient(client_id);
            }
            else if (clientMessage == "move_right")
            {
                game.moveCurrentPieceRight();
                updateGameForClient(client_id);
            }
            else if (clientMessage == "rotate")
            {
                game.rotateCurrentPiece();
                updateGameForClient(client_id);
            }
            else if (clientMessage == "rotate")
            {
                game.moveCurrentPieceDown();
                updateGameForClient(client_id);
            }
            // Additional game actions can be handled here
            break;
            // Handle other message types as needed
        }
    }

    {
        std::lock_guard<std::mutex> lock(clientSocketsMutex);
        clientSockets.erase(client_id);
    }
    close(clientSocket);
}

std::string Server::receiveMessage(int clientSocket, MessageHeader &header)
{
    ssize_t bytesRead = recv(clientSocket, &header, sizeof(header), 0);
    if (bytesRead <= 0)
    {
        return "";
    }

    std::string payload(header.messageLength, '\0');
    bytesRead = recv(clientSocket, &payload[0], header.messageLength, 0);
    if (bytesRead <= 0)
    {
        return "";
    }

    return payload;
}

void Server::startGameForClient(unsigned long long client_id)
{
    printf("this is startGameForClient function in server\n");
    // std::lock_guard<std::mutex> lock(clientGamesMutex);
    printf("start to find the game for client\n");
    auto gameIter = clientGames.find(client_id);
    printf("check1\n");
    if (gameIter == clientGames.end())
    {
        printf("check2\n");
        gameIter = clientGames.emplace(client_id, Tetris::Game()).first;
    }
    printf("check3\n");
    Tetris::Game &game = gameIter->second;
    printf("call game.startNewGame\n");
    game.startNewGame();
    printf("check4\n");
    std::string gameState = game.getCurrentState();
    printf("check5\n");
    std::cout << gameState << std::endl;
    sendMessageToClient(client_id, MessageType::GAME_STATE, gameState);
}

void Server::sendMessage(int clientSocket, const MessageHeader &header, const std::string &payload)
{
    printf("send message from server\n");
    send(clientSocket, &header, sizeof(header), 0);
    send(clientSocket, payload.c_str(), payload.size(), 0);
}

void Server::handleIDResponse(int clientSocket, const MessageHeader &header, unsigned long long client_id)
{
    // Process client ID (you can store it, log it, etc.)
    std::cout << "Received ID response from client " << header.clientID << std::endl;
}

void Server::handleGameAction(int clientSocket, const MessageHeader &header, Tetris::Game &game, unsigned long long client_id)
{
    // Process game action from client

    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, header.messageLength, 0);
    if (bytesRead > 0)
    {
        std::string action(buffer, bytesRead);
        if (action == "MOVE_LEFT")
            game.moveCurrentPieceLeft();
        if (action == "MOVE_RIGHT")
            game.moveCurrentPieceRight();
        if (action == "ROTATE")
            game.rotateCurrentPiece();
        if (action == "MOVE_DOWN")
            game.moveCurrentPieceDown();
    }
}

void Server::sendGameState(int clientSocket, Tetris::Game &game)
{
    std::string gameState = game.getCurrentState();
    MessageHeader header;
    header.messageType = MessageType::GAME_STATE;
    header.messageLength = gameState.size();
    header.clientID = 0;

    send(clientSocket, &header, sizeof(header), 0);
    send(clientSocket, gameState.c_str(), gameState.size(), 0);
}

void Server::sendGameOver(int clientSocket)
{
    std::string message = "Game Over";
    MessageHeader header;
    header.messageType = MessageType::GAME_OVER;
    header.messageLength = message.size();
    header.clientID = 1;

    send(clientSocket, &header, sizeof(header), 0);
    send(clientSocket, message.c_str(), message.size(), 0);
};

void Server::updateGameForClient(unsigned long long client_id)
{
    std::lock_guard<std::mutex> lock(clientGamesMutex);

    auto gameIter = clientGames.find(client_id);
    if (gameIter == clientGames.end())
    {
        std::cerr << "Game instance not found for client ID: " << client_id << std::endl;
        return;
    }

    Tetris::Game &game = gameIter->second;
    if (!game.isGameOver())
    {
        game.updateGame(); // This should internally handle score and level updates

        std::string gameState = game.getCurrentState();
        sendMessageToClient(client_id, MessageType::GAME_STATE, gameState);
    }
    else
    {
        sendGameOver(client_id);
    }
}

void Server::sendMessageToClient(unsigned long long client_id, MessageType messageType, const std::string &payload)
{
    printf("start send that message to client\n");
    auto socketIter = clientSockets.find(client_id);
    if (socketIter != clientSockets.end())
    {
        int clientSocket = socketIter->second;
        MessageHeader header = {messageType, static_cast<int>(payload.size()), static_cast<int>(client_id)};
        sendMessage(clientSocket, header, payload);
    }
    else
    {
        std::cerr << "Client socket not found for ID: " << client_id << std::endl;
    }
}
