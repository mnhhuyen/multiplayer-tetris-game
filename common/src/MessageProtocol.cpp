#include "../include/MessageProtocol.hpp"
#include <string.h>

Message pack_message(MessageType messageType, int clientID, const std::string& payload) {
    return Message(messageType, clientID, payload);
}


void unpack_message(const Message& message, MessageType& messageType, int& clientID, std::string& payload) {
    messageType = message.messageHeader.messageType;
    clientID = message.messageHeader.clientID;
    payload = message.payload;
}


