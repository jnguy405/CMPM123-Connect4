#include "Connect4.h"


const int CONNECT4_ROWS = 6;
const int CONNECT4_COLS = 7;

Connect4::Connect4()
{
}

Connect4::~Connect4()
{
}


Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "red.png" or the "yellow.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == 0 ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = CONNECT4_ROWS;
    _gameOptions.rowY = CONNECT4_COLS;
    //_grid = new Grid(CONNECT4_ROWS, CONNECT4_COLS, 80, 80, ImVec2(100, 100));
    

    return;
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {
    return false;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src) {
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    return false;
}

void Connect4::stopGame()
{
}

Player* Connect4::ownerAt(int index ) const
{
    return nullptr;
}

Player* Connect4::checkForWinner()
{
    return nullptr;
}

bool Connect4::checkForDraw()
{
    return false;
}

std::string Connect4::initialStateString()
{
    std::string state = "";
    for (int i = 0; i < CONNECT4_ROWS * CONNECT4_COLS; i++)
    {
        state += "0"; // Initialize each position as empty (represented by '0')
    }

    // state should be a string of 42 zeros
    return state;
}

std::string Connect4::stateString()
{
    return "";
}

void Connect4::setStateString(const std::string &s)
{
    return;
}

void Connect4::updateAI()
{
    return;
}

bool Connect4::gameHasAI()
{
    return true;
}

bool Connect4::aiTestForTerminalState(std::string &state, Player *&winner)
{
    return false;
}

int Connect4::aiBoardEvaluation(const std::string& state)
{
    return 0;
}

void Connect4::negamax(std::string& state, int depth, int playerColor)
{
    return;
}

