find_package(Qt6 COMPONENTS Widgets REQUIRED)

file(GLOB SERVER_SOURCES src/*.cpp)
file(GLOB SERVER_HEADERS include/*.hpp)

add_executable(tetris_server ${SERVER_SOURCES} ${SERVER_HEADERS})
target_include_directories(tetris_server PRIVATE include)
target_link_libraries(tetris_server PRIVATE Qt6::Widgets tetris_common)
