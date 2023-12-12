#ifndef CLIENTNETWORKHANDLER_HPP
#define CLIENTNETWORKHANDLER_HPP

#include "../../common/include/MessageProtocol.hpp"

#include <string>
#include <thread>
#include <atomic>
#include <netinet/in.h>

class ClientNetworkHandler {
public:
    ClientNetworkHandler(const std::string& serverIP, int serverPort);
    ~ClientNetworkHandler();

    void start();
    void stop();
    void sendUserInput(const std::string& input);
    void sendMessage(const MessageHeader& header, const std::string& payload);
    std::string receiveMessage(MessageHeader& header);

private:
    std::thread networkThread;
    std::atomic<bool> running;

    int sockfd;
    struct sockaddr_in servaddr;

    void run();
    void setupSocket();
    std::string receiveGameState();
};

#endif // CLIENTNETWORKHANDLER_HPP
