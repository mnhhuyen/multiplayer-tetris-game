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

    bool clientConnected = true;
    while (clientConnected)
    {
        MessageHeader header;
        std::string clientMessage = receiveMessage(clientSocket, header);

        if (clientMessage.empty())
        {
            clientConnected = false;
            break;
        }

        auto gameIter = clientGames.find(client_id);
        if (gameIter == clientGames.end())
        {
            // No game found for this client, handle this case appropriately
            continue;
        }
        Tetris::Game &game = gameIter->second;

        switch (header.messageType)
        {
        case MessageType::GAME_ACTION:
            if (clientMessage == "START")
            {
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
    std::lock_guard<std::mutex> lock(clientGamesMutex); // Lock the mutex for thread safety

    // Check if the client already has a game instance, and create one if not
    auto gameIter = clientGames.find(client_id);
    if (gameIter == clientGames.end())
    {
        // Insert a new Game instance for this client
        gameIter = clientGames.emplace(client_id, Tetris::Game()).first;
    }

    // Start or reset the game for this client
    Tetris::Game &game = gameIter->second;
    game.startNewGame();

    // Unlock the mutex automatically when lock_guard goes out of scope

    // Serialize the initial game state
    std::string gameState = game.getCurrentState(); // Ensure this method properly serializes the game state

    // Prepare the message to be sent to the client
    MessageHeader header;
    header.messageType = MessageType::GAME_STATE; // Adjust as per your protocol
    header.clientID = client_id;                  // Set the client ID
    header.messageLength = gameState.size();      // Set the length of the game state

    // Find the socket associated with this client
    auto socketIter = clientSockets.find(client_id);
    if (socketIter != clientSockets.end())
    {
        int clientSocket = socketIter->second;

        // Send the game state to the client
        sendMessage(clientSocket, header, gameState);
    }
    else
    {
        std::cerr << "Failed to find socket for client ID " << client_id << std::endl;
    }
}

void Server::sendMessage(int clientSocket, const MessageHeader &header, const std::string &payload)
{
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
    auto socketIter = clientSockets.find(client_id);
    if (socketIter != clientSockets.end())
    {
        int clientSocket = socketIter->second;

        // Prepare the message header
        MessageHeader header;
        header.messageType = messageType;
        header.clientID = client_id;
        header.messageLength = payload.size();

        // Use the existing sendMessage function
        sendMessage(clientSocket, header, payload);
    }
    else
    {
        std::cerr << "Client socket not found for ID: " << client_id << std::endl;
    }
}
