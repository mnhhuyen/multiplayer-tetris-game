#include <iostream>
#include "../include/Server.h"

int main() {
    int port = 5000; 
    Server server(port);  
    server.start();      

    std::string input;
    while (true) {
        std::cin >> input;
        if (input == "exit") {
            break;
        }
    }

    server.stop();
    return 0;
}

