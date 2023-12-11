cmake_minimum_required(VERSION 3.5)

project(Tetris LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/home/nmh/Qt/6.6.0/gcc_64")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for client, server, and common
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
