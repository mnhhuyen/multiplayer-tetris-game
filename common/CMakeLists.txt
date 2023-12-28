find_package(Qt6 COMPONENTS Widgets REQUIRED)
add_library(tetris_common STATIC
    src/Tetromino.cpp
    src/Tetromino_I.cpp
    src/Tetromino_J.cpp
    src/Tetromino_L.cpp
    src/Tetromino_O.cpp
    src/Tetromino_S.cpp
    src/Tetromino_T.cpp
    src/Tetromino_Z.cpp
    src/Board.cpp
    src/TetrominoFactory.cpp
    src/GameState.cpp
)

target_include_directories(tetris_common PUBLIC include)
target_link_libraries(tetris_common PRIVATE Qt6::Widgets)