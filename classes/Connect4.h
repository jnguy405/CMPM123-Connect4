#pragma once
#include "Game.h"

class Connect4 : public Game
{
public:
    Connect4();
    ~Connect4();

    // set up the board
    void setUpBoard() override;
    bool actionForEmptyHolder(BitHolder &holder) override;
    bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void stopGame() override;
    Player* checkForWinner() override;
    bool checkForDraw() override;
    
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    
    // Grid accessor
    Grid* getGrid() override { return _grid; }
    
    // AI methods
    bool gameHasAI() override;
    void updateAI() override;
    
    // Helper methods
    int getBestMoveColumn() const { return _bestMoveColumn; }
    void setAIPlayer(int playerNumber, bool isAI);
    
    // AI evaluation methods
    bool aiTestForTerminalState(std::string &state, Player *&winner);
    int negamax(std::string &state, int depth, int playerColor, char aiChar, char opponentChar);
    int aiBoardEvaluation(const std::string &state, char aiChar, char opponentChar);

private:
    Grid *_grid;
    int _bestMoveColumn;
    
    Bit* PieceForPlayer(const int playerNumber);
};