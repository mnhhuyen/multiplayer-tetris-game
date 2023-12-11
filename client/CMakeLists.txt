set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

file(GLOB CLIENT_SOURCES src/*.cpp src/gui/*.cpp)
file(GLOB CLIENT_HEADERS include/gui/*.hpp)

add_executable(tetris_client ${CLIENT_SOURCES} ${CLIENT_HEADERS})
target_include_directories(tetris_client PRIVATE include)
target_link_libraries(tetris_client PRIVATE Qt6::Widgets Qt6::OpenGLWidgets tetris_common)
