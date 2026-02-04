#include "Connect4.h"


const int CONNECT4_ROWS = 6;
const int CONNECT4_COLS = 7;

Connect4::Connect4()
{
    _grid = new Grid(CONNECT4_COLS, CONNECT4_ROWS);
}

Connect4::~Connect4()
{
    delete _grid;
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
    
    // Initialize each square in the grid
    for (int y = 0; y < CONNECT4_ROWS; y++)
    {
        for (int x = 0; x < CONNECT4_COLS; x++)
        {
            ImVec2 position(100.0f + x * 90.0f, 100.0f + y * 90.0f);
            ChessSquare* square = _grid->getSquare(x, y);
            if (square) {
                square->initHolder(position, "", x, y);
            }
        }
    }
    return;
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {
    // check for null holder

    // get current player index
    
    // place piece for current player in holder

    // return true
    return false;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src) {
    // implement for piece animation
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    // implement for piece animation
    return false;
}

void Connect4::stopGame()
{
    // clear the board
    // loop the board to destroybit on each square
}

Player* Connect4::ownerAt(int index ) const
{
    // get the bit at the index and return its owner
    // if not bit return nullptr
    // return bit->getOwner();
    return nullptr;
}

Player* Connect4::checkForWinner()
{
    // check each winning diagonal / horizontal / vertical

    // iterate diagonals -1 to +1 of the main diagonal
    // for each diagonal, check for 4 in a row for each player

    // iterate horizontals
    // for each horizontal, check for 4 in a row for each player

    // iterate verticals
    // for each vertical, check for 4 in a row for each player

    // if found, return the winning player
    return nullptr;
}

bool Connect4::checkForDraw()
{
    // if there's a winner, it's not a draw
    // check if board is full
    // if any square is empty, it's not a draw yet
    return false;
    // return true if board is full with no winner
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
    // loop through the grid and build a string representation of the board
    // for each square, append '0' for empty, '1' for player 1, '2' for player 2

    // check for case of ai is player 1 or 2 and swap the representation accordingly
    // return the resulting string
    return "";
}

void Connect4::setStateString(const std::string &s)
{
    // for each character in the string, set the corresponding square in the grid
    // '0' = empty, '1' = player 1 piece, '2
    // create piece for player and set in holder
    return;
}

void Connect4::updateAI()
{
    // implement a simple AI for Connect4 using minimax or negamax algorithm
    return;
}

bool Connect4::gameHasAI()
{
    // check if any player is an AI player
    return true;
}

bool Connect4::aiTestForTerminalState(std::string &state, Player *&winner)
{
    // get the npos of '0' in the state string
    // if npos is std::string::npos then board is full
    return false;
}

int Connect4::aiBoardEvaluation(const std::string& state)
{
    // chec for winner
    // check if player 1 has won return positive score
    // check if player 2 has won return negative score
    return 0;
}

int Connect4::negamax(std::string& state, int depth, int playerColor)
{
    // check for winner or draw
    // if terminal state return evaluation score

    // for each possible move
    // make the move
    // call negamax recursively with increased depth and switched player color
    // undo the move

    // based on scores return the best move
    return;
}

