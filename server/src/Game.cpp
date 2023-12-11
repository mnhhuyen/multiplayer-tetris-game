#include "../include/Game.h"
#include "../../common/include/TetrominoFactory.hpp"
#include <stdexcept>

namespace Tetris {

Game::Game()
    : board(), 
      currentPiece(Tetris::core::TetrominoFactory::UniformPieceRandomizer()), 
      nextPiece(Tetris::core::TetrominoFactory::UniformPieceRandomizer()), 
      score(0), level(1), gameOver(false), linesCleared(0) {
    board.setCurrentPiece(std::move(currentPiece));
    board.setNextPiece(std::move(nextPiece));
}

void Game::startNewGame() {
    board.clear();
    currentPiece = Tetris::core::TetrominoFactory::UniformPieceRandomizer();
    nextPiece = Tetris::core::TetrominoFactory::UniformPieceRandomizer();
    board.setCurrentPiece(std::move(currentPiece));
    board.setNextPiece(std::move(nextPiece));
    gameOver = false;
    score = 0;
    level = 1;
    linesCleared = 0;
}

void Game::updateGame() {
    if (gameOver) return;

    if (!board.canMoveCurrentPieceDown()) {
        board.dropCurrentPiece();
        swapPieces();
        auto completedRange = board.hasCompletedLines();
        if (completedRange.first && completedRange.second) {
            int lines = completedRange.second - completedRange.first;
            linesCleared += lines;
            updateScore(lines);
            checkLevelUp();
            board.eraseLines(completedRange);
        }

        if (board.isGameOver()) {
            gameOver = true;
        }
    } else {
        auto* current = board.getCurrentPiece();
        current->setY(current->getY() + 1);
    }
}

void Game::updateScore(int linesCleared) {
    static const int scores[] = {0, 40, 100, 300, 1200};
    if (linesCleared >= 1 && linesCleared <= 4) {
        score += scores[linesCleared] * level;
    }
}

void Game::checkLevelUp() {
    if (linesCleared / 10 > level) {
        level++;
        // Adjust game speed or other level-related factors here
    }
}

void Game::moveCurrentPieceLeft() {
    if (board.canMoveCurrentPieceLeft()) {
        auto* current = board.getCurrentPiece();
        current->setX(current->getX() - 1);
    }
}

void Game::moveCurrentPieceRight() {
    if (board.canMoveCurrentPieceRight()) {
        auto* current = board.getCurrentPiece();
        current->setX(current->getX() + 1);
    }
}

void Game::rotateCurrentPiece() {
    if (board.canRotateCurrentPiece()) {
        auto* current = board.getCurrentPiece();
        current->setOrientation((current->getOrientation() + 1) % 4);
    }
}

void Game::swapPieces() {
    currentPiece = std::move(nextPiece);
    nextPiece = Tetris::core::TetrominoFactory::UniformPieceRandomizer();
    board.setCurrentPiece(std::move(currentPiece));
    board.setNextPiece(std::move(nextPiece));
}

bool Game::isGameOver() const {
    return gameOver;
}

std::string Game::getCurrentState() const {
    // Implement serialization logic for game state
    return "";
}
}