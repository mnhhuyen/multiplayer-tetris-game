#ifndef MESSAGEPROTOCOL_HPP
#define MESSAGEPROTOCOL_HPP

#include <string>

enum class MessageType {
    ID_REQUEST = 0,
    ID_RESPONSE,
    GAME_ACTION,
    ACCEPTED,
    GAME_OVER,
    GAME_STATE
};

struct MessageHeader {
    MessageType messageType;
    int messageLength;
    int clientID;
};

struct Message {
    MessageHeader messageHeader;
    std::string payload;

    Message(MessageType type, int clientID, const std::string& data)
        : messageHeader{type, static_cast<int>(data.size()), clientID}, payload(data) {}
};

Message pack_message(MessageType messageType, int clientID, const std::string& payload);
void unpack_message(const Message& message, MessageType& messageType, int& clientID, std::string& payload);

#endif // MESSAGEPROTOCOL_HPP
