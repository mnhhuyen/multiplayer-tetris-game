#include "Tetromino.hpp"
#include "Tetromino_I.hpp"
#include "Tetromino_J.hpp"
#include "Tetromino_L.hpp"
#include "Tetromino_O.hpp"
#include "Tetromino_S.hpp"
#include "Tetromino_T.hpp"
#include "Tetromino_Z.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>
Tetris::core::Tetromino::Tetromino():
    m_x(3),
    m_y(0),
    m_orientation(0)
{

}

void Tetris::core::Tetromino::setX(const int x){
    m_x = x;
}

void Tetris::core::Tetromino::setY(const int y){
    m_y = y;
}

int Tetris::core::Tetromino::getX() const{
    return m_x;
}

int Tetris::core::Tetromino::getY() const{
    return m_y;
}

int Tetris::core::Tetromino::getOrientation() const{
    return m_orientation;
}

std::array<std::array<char,4>,4> Tetris::core::Tetromino::getPiece() const{
    return getPiece(m_orientation);
}

void Tetris::core::Tetromino::setOrientation(const int x){
    m_orientation = x;
}

std::string Tetris::core::Tetromino::serialize() const {
    printf("this is serialize function\n");
    std::ostringstream out;
    // std::cout << m_x << m_y << m_orientation;
    out << m_x << "," << m_y << "," << m_orientation << "," << getChar();
    printf("start to serialize piece\n");
    auto piece = getPiece();
    for (const auto& row : piece) {
        for (char cell : row) {
            out << "," << cell;
        }
    }

    return out.str();
}

std::unique_ptr<Tetris::core::Tetromino> Tetris::core::Tetromino::deserialize(const std::string& serializedData) {
    std::istringstream in(serializedData);
    int x, y, orientation;
    char type, delimiter;
    std::array<std::array<char, 4>, 4> pieceArray;

    in >> x >> delimiter >> y >> delimiter >> orientation >> delimiter >> type;

    std::unique_ptr<Tetris::core::Tetromino> tetromino;

    switch (type) {
        case I_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_I>(); break;
        case J_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_J>(); break;
        case L_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_L>(); break;
        case O_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_O>(); break;
        case S_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_S>(); break;
        case T_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_T>(); break;
        case Z_CHAR: tetromino = std::make_unique<Tetris::core::Tetromino_Z>(); break;
        default: return nullptr;
    }

    if (!tetromino) {
        // Handle error or return null
        return nullptr;
    }

    tetromino->setX(x);
    tetromino->setY(y);
    tetromino->setOrientation(orientation);

    // Deserialize the piece array
    for (auto& row : pieceArray) {
        for (char& cell : row) {
            if (!(in >> delimiter >> cell)) {
                // Handle error
                return nullptr;
            }
        }
    }

    // Assuming Tetromino has a method to set the piece array
    // tetromino->setPieceArray(pieceArray);

    return tetromino;
}
