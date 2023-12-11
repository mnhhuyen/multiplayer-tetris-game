#include "ClientNetworkHandler.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

ClientNetworkHandler::ClientNetworkHandler(const std::string& serverIP, int serverPort)
    : running(false) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP.c_str(), &servaddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }
}

ClientNetworkHandler::~ClientNetworkHandler() {
    stop();
    close(sockfd);
}

void ClientNetworkHandler::start() {
    running = true;
    networkThread = std::thread(&ClientNetworkHandler::run, this);
}

void ClientNetworkHandler::stop() {
    running = false;
    if (networkThread.joinable())
        networkThread.join();
}

void ClientNetworkHandler::run() {
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (running) {
        std::string gameState = receiveGameState();
        // Process game state (e.g., update UI)
    }
}

void ClientNetworkHandler::sendUserInput(const std::string& input) {
    send(sockfd, input.c_str(), input.size(), 0);
}

std::string ClientNetworkHandler::receiveGameState() {
    char buffer[1024] = {0};
    ssize_t bytesRead = read(sockfd, buffer, 1024);
    if (bytesRead > 0) {
        return std::string(buffer, bytesRead);
    }
    return "";
}

void ClientNetworkHandler::sendMessage(const MessageHeader& header, const std::string& payload) {
    send(sockfd, &header, sizeof(header), 0);
    send(sockfd, payload.c_str(), payload.length(), 0);
}

std::string ClientNetworkHandler::receiveMessage(MessageHeader& header) {
    recv(sockfd, &header, sizeof(header), 0);

    std::string payload(header.messageLength, '\0');
    recv(sockfd, &payload[0], header.messageLength, 0);
    return payload;
}