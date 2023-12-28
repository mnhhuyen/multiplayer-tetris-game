#include "../include/GameState.hpp"
#include "../include/Board.hpp"
namespace Tetris::core
{

    GameState::GameState()
        : board(nullptr), score(0), linesCleared(0), level(0), isGameOver(false)
    {
    }

    void GameState::setBoard(std::unique_ptr<Tetris::core::Board> newBoard)
    {
        board = std::move(newBoard);
    }

    const Board *GameState::getBoard() const
    {
        return board.get();
    }

    void GameState::setCurrentPiece(std::unique_ptr<Tetromino> piece)
    {
        currentPiece = std::move(piece);
    }

    Tetromino *GameState::getCurrentPiece() const
    {
        return currentPiece.get();
    }

    void GameState::setNextPiece(std::unique_ptr<Tetromino> piece)
    {
        nextPiece = std::move(piece);
    }

    Tetromino *GameState::getNextPiece() const
    {
        return nextPiece.get();
    }

    void GameState::setScore(int newScore)
    {
        score = newScore;
    }

    int GameState::getScore() const
    {
        return score;
    }

    void GameState::setLinesCleared(int newLines)
    {
        linesCleared = newLines;
    }

    int GameState::getLinesCleared() const
    {
        return linesCleared;
    }

    void GameState::setLevel(int newLevel)
    {
        level = newLevel;
    }

    int GameState::getLevel() const
    {
        return level;
    }

    void GameState::setIsGameOver(bool newIsGameOver)
    {
        isGameOver = newIsGameOver;
    }

    bool GameState::getIsGameOver() const
    {
        return isGameOver;
    }

}
