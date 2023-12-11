// server/src/main.cpp
#include "Server.h"

int main() {
    int port = 5000;  // Port number for the server
    Server server(port);
    server.start();
    return 0;
}
