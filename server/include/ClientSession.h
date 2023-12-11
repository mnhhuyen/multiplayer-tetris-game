// server/include/ClientSession.h
#pragma once
#include "Game.h"

class ClientSession {
public:
    ClientSession(int socket);
    void start();

private:
    int client_socket;
    Tetris::Game game;

    void receiveInput();
    void sendUpdate();
};
