#include "../include/ClientNetworkHandler.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

ClientNetworkHandler::ClientNetworkHandler(const std::string &serverIP, int serverPort)
    : running(false)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP.c_str(), &servaddr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }
}

ClientNetworkHandler::~ClientNetworkHandler()
{
    stop();
    close(sockfd);
}

void ClientNetworkHandler::setClientID(int id)
{
    clientID = id;
}

void ClientNetworkHandler::start()
{
    running = true;
    networkThread = std::thread(&ClientNetworkHandler::run, this);
}

void ClientNetworkHandler::stop()
{
    running = false;
    if (networkThread.joinable())
        networkThread.join();
}

void ClientNetworkHandler::run()
{
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        running = false;
        return;
    }

    // Connection is successful
    std::cout << "Successfully connected to the server." << std::endl;

    while (running) {
        std::string gameState = receiveGameState();  // This needs to be adapted based on your protocol
        if (!gameState.empty()) {
            // Process the game state
            // Update UI or game state
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void ClientNetworkHandler::sendUserInput(const std::string &input)
{
    MessageHeader header;
    header.messageType = MessageType::GAME_ACTION; // Adjust the message type if necessary
    header.clientID = this->clientID;              // Make sure this is your client's unique ID
    header.messageLength = input.length();

    sendMessage(header, input);
}

std::string ClientNetworkHandler::receiveGameState()
{
    char buffer[1024] = {0};
    ssize_t bytesRead = read(sockfd, buffer, 1024);
    if (bytesRead > 0)
    {
        return std::string(buffer, bytesRead);
    }
    return "";
}

void ClientNetworkHandler::sendMessage(const MessageHeader &header, const std::string &payload) {
    send(sockfd, &header, sizeof(header), 0);
    send(sockfd, payload.c_str(), payload.length(), 0);
}

std::string ClientNetworkHandler::receiveMessage(MessageHeader& header) {
    ssize_t bytesRead = recv(sockfd, &header, sizeof(header), 0);
    if (bytesRead <= 0) {
        std::cerr << "Failed to read message header or connection closed" << std::endl;
        return "";
    }

    std::string payload(header.messageLength, '\0');
    bytesRead = recv(sockfd, &payload[0], header.messageLength, 0);
    if (bytesRead <= 0) {
        std::cerr << "Failed to read message payload or connection closed" << std::endl;
        return "";
    }

    return payload;
}