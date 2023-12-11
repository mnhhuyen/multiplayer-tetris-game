#ifndef MESSAGEPROTOCOL_HPP
#define MESSAGEPROTOCOL_HPP

#include <string>

// Define message types
enum class MessageType {
    ID_REQUEST = 0,
    ID_RESPONSE,
    GAME_ACTION,
    ACCEPTED,
    GAME_OVER,
    GAME_STATE,
    // ... Add other message types as needed
};

// Define the message header structure
struct MessageHeader {
    MessageType messageType;
    int messageLength;
    int senderID;
    int receiverID;
};

#endif // MESSAGEPROTOCOL_HPP
