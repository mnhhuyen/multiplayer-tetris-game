#ifndef CLIENTNETWORKHANDLER_HPP
#define CLIENTNETWORKHANDLER_HPP

#include "../../common/include/MessageProtocol.hpp"

#include <string>
#include <thread>
#include <atomic>
#include <netinet/in.h>
#include <QObject>

class ClientNetworkHandler : public QObject
{
    Q_OBJECT
    int clientID;

public:
    ClientNetworkHandler(const std::string &serverIP, int serverPort);
    ~ClientNetworkHandler();

    void start();
    void stop();
    void setClientID(int id);
    void sendUserInput(const std::string &input);
    void sendMessage(const MessageHeader &header, const std::string &payload);
    std::pair<MessageHeader, std::string>receiveMessage();
    std::string receiveGameState();

private:
    std::thread networkThread;
    std::atomic<bool> running;

    int sockfd;
    struct sockaddr_in servaddr;

    void run();
    void setupSocket();

signals:
    void gameStateReceived(const std::string& gameState);
    // std::string receiveGameState();
};

#endif // CLIENTNETWORKHANDLER_HPP
