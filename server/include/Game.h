// server/include/Game.h
#pragma once
#include <vector>
#include <memory>
#include "../../common/include/Tetromino.hpp" 
#include "../../common/include/Board.hpp"    

namespace Tetris
{
    using core::Board;
    using core::Tetromino;
    class Game
    {
    public:
        Game();
        void startNewGame();
        void updateGame(); // Called periodically to update game state
        void swapPieces();
        void moveCurrentPieceDown();
        void rotateCurrentPiece();
        void moveCurrentPieceLeft();
        void moveCurrentPieceRight();
        bool isGameOver() const;
        std::string getCurrentState() const;

    private:
        Board board;            // Represents the Tetris board
        std::unique_ptr<Tetromino> currentPiece;
        std::unique_ptr<Tetromino> nextPiece;
        int score;
        int level;
        bool gameOver;
        int linesCleared;

        void checkLineCompletion();
        void updateScore(int linesCleared);
        void generateNewPiece();
        void swapCurrentAndNextPiece();
        void checkLevelUp();
    };

}
