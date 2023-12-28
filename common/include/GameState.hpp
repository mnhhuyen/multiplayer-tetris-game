#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "Tetromino.hpp"
#include <memory>

namespace Tetris::core {

    class Board; // Forward declaration

    class GameState {
    public:
        GameState();

        // Use raw pointer for Board to avoid copy
        void setBoard(std::unique_ptr<Tetris::core::Board> board);

        const Board* getBoard() const;

        // Use unique_ptr for Tetrominos
        void setCurrentPiece(std::unique_ptr<Tetromino> piece);
        Tetromino* getCurrentPiece() const;

        void setNextPiece(std::unique_ptr<Tetromino> piece);
        Tetromino* getNextPiece() const;

        // Game metrics
        void setScore(int newScore);
        int getScore() const;

        void setLinesCleared(int newLines);
        int getLinesCleared() const;

        void setLevel(int newLevel);
        int getLevel() const;

        void setIsGameOver(bool newIsGameOver);
        bool getIsGameOver() const;

    private:
        std::unique_ptr<Tetris::core::Board> board;
        std::unique_ptr<Tetromino> currentPiece;
        std::unique_ptr<Tetromino> nextPiece;
        int score;
        int linesCleared;
        int level;
        bool isGameOver;
    };

}

#endif // GAMESTATE_HPP
