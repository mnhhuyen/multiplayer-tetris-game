#include "../include/Game.h"
#include "../../common/include/TetrominoFactory.hpp"
#include "../../common/include/Board.hpp"
#include <stdexcept>
#include <iostream>

namespace Tetris
{

    Game::Game()
        : board(),
          currentPiece(Tetris::core::TetrominoFactory::UniformPieceRandomizer()),
          nextPiece(Tetris::core::TetrominoFactory::UniformPieceRandomizer()),
          score(0), level(1), gameOver(false), linesCleared(0)
    {
        board.setCurrentPiece(std::move(currentPiece));
        board.setNextPiece(std::move(nextPiece));
    }

    void Game::startNewGame()
    {
        board.clear();
        currentPiece = Tetris::core::TetrominoFactory::UniformPieceRandomizer();
        nextPiece = Tetris::core::TetrominoFactory::UniformPieceRandomizer();
        board.setCurrentPiece(std::move(currentPiece));
        board.setNextPiece(std::move(nextPiece));
        gameOver = false;
        score = 0;
        level = 0;
        linesCleared = 0;
    }

    std::string Game::getCurrentState()
    {
        std::string state;
        printf("this is in getCurrentState function\n");

        // Serialize the board layout
        const std::array<std::array<char, Tetris::core::Board::m_width>, Tetris::core::Board::m_height> &boardArray = board.getBoard();
        for (const auto &row : boardArray)
        {
            for (char cell : row)
            {
                state += cell;
            }
            state += '\n';
        }
        std::cout << "board state" << state << std::endl;
        auto currentPiece = board.getCurrentPiece();
        auto nextPiece = board.getNextPiece();
        state += currentPiece->serialize() + "\n";
        state += nextPiece->serialize() + "\n";

        std::cout << "serialized state" << state << std::endl;

        state += "Score: " + std::to_string(score) + "\n";
        state += "Level: " + std::to_string(level) + "\n";
        state += "Lines Cleared: " + std::to_string(linesCleared) + "\n";

        return state;
    }

    void Game::updateGame()
    {
        if (gameOver)
            return;

        if (!board.canMoveCurrentPieceDown())
        {
            board.dropCurrentPiece();
            swapPieces();
            auto completedRange = board.hasCompletedLines();
            if (completedRange.first && completedRange.second)
            {
                int lines = completedRange.second - completedRange.first;
                linesCleared += lines;
                updateScore(lines); // Update the score based on lines cleared
                checkLevelUp();     // Check and update level if needed
                board.eraseLines(completedRange);
            }

            if (board.isGameOver())
            {
                gameOver = true;
            }
        }
        else
        {
            auto *current = board.getCurrentPiece();
            current->setY(current->getY() + 1);
        }
    }

    // Combine the score and level update logic into single functions
    void Game::updateScore(int linesCleared)
    {
        static const int scores[] = {0, 40, 100, 300, 1200};
        if (linesCleared >= 1 && linesCleared <= 4)
        {
            score += scores[linesCleared] * level;
        }
    }

    void Game::checkLevelUp()
    {
        if (this->linesCleared / 10 > level)
        {
            level++;
            // Adjust game speed or other level-related factors here
        }
    }

    void Game::moveCurrentPieceLeft()
    {
        if (board.canMoveCurrentPieceLeft())
        {
            auto *current = board.getCurrentPiece();
            current->setX(current->getX() - 1);
        }
    }

    void Game::moveCurrentPieceRight()
    {
        if (board.canMoveCurrentPieceRight())
        {
            auto *current = board.getCurrentPiece();
            current->setX(current->getX() + 1);
        }
    }

    void Game::rotateCurrentPiece()
    {
        if (board.canRotateCurrentPiece())
        {
            auto *current = board.getCurrentPiece();
            current->setOrientation((current->getOrientation() + 1) % 4);
        }
    }

    void Game::moveCurrentPieceDown()
    {
        if (board.canMoveCurrentPieceDown())
        {
            auto *current = board.getCurrentPiece();
            current->setY(current->getY() + 1);
        }
    }

    void Game::swapPieces()
    {
        currentPiece = std::move(nextPiece);
        nextPiece = Tetris::core::TetrominoFactory::UniformPieceRandomizer();
        board.setCurrentPiece(std::move(currentPiece));
        board.setNextPiece(std::move(nextPiece));
    }

    bool Game::isGameOver() const
    {
        return gameOver;
    }

    // std::string Game::getCurrentState() const {
    //     // Implement serialization logic for game state
    //     return "";
    // }

    std::pair<int, int> Game::checkForCompletedLines()
    {
        return board.hasCompletedLines();
    }

    // void Game::updateScoreAndLevel(const std::pair<int, int>& completedRange) {
    //     if (completedRange.first != -1 && completedRange.second != -1) {
    //         int linesCleared = completedRange.second - completedRange.first + 1;
    //         linesCleared += linesCleared;

    //         // Update score based on number of lines cleared and current level
    //         static const int scores[] = {0, 40, 100, 300, 1200};
    //         if (linesCleared >= 1 && linesCleared <= 4) {
    //             score += scores[linesCleared] * (level + 1);
    //         }

    //         // Check for level up
    //         if (linesCleared / 10 > level) {
    //             level++;
    //             // Optionally adjust game speed or other level-related factors here
    //         }

    //         // Erase the completed lines from the board
    //         board.eraseLines(completedRange);
    //     }
    // }

}
