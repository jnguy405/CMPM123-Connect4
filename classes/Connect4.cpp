#include "Connect4.h"
#include <climits>

const int CONNECT4_COLS = 7;
const int CONNECT4_ROWS = 6;

Connect4::Connect4() : Game()
{
    _grid = new Grid(CONNECT4_COLS, CONNECT4_ROWS);
    _bestMoveColumn = 0;
}

Connect4::~Connect4()
{
    delete _grid;
}

Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    Bit* bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 0 ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = CONNECT4_COLS;
    _gameOptions.rowY = CONNECT4_ROWS;
    
    // Initialize all squares with proper positioning
    _grid->initializeSquares(80.0f, "square.png");

    startGame();
}

// Gravity: pieces drop to the lowest empty row in a column
bool Connect4::actionForEmptyHolder(BitHolder &holder) {
    ChessSquare* square = static_cast<ChessSquare*>(&holder);
    if (!square) return false;

    int col = square->getColumn();
    
    // Find the lowest empty row in this column
    int dropRow = -1;
    for (int y = CONNECT4_ROWS - 1; y >= 0; y--) {
        ChessSquare* target = _grid->getSquare(col, y);
        if (target && !target->bit()) {
            dropRow = y;
            break;
        }
    }

    if (dropRow == -1) return false; // Column is full

    ChessSquare* dropSquare = _grid->getSquare(col, dropRow);
    if (!dropSquare) return false;

    // Create piece for current player
    Bit* piece = PieceForPlayer(getCurrentPlayer() == getPlayerAt(0) ? 0 : 1);
    piece->setPosition(dropSquare->getPosition());
    dropSquare->setBit(piece);

    // Check for win/draw before ending turn
    Player* winner = checkForWinner();
    if (winner) {
        _winner = winner;
        stopGame();
        return true;
    }

    if (checkForDraw()) {
        stopGame();
        return true;
    }

    endTurn();
    return true;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src) {
    // Pieces in Connect4 can't be picked up and moved
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    // Pieces in Connect4 can't be picked up and moved
    return false;
}

void Connect4::stopGame()
{
    // Clear the board
    if (_grid) {
        _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            if (square && square->bit()) {
                square->destroyBit();
            }
        });
    }
}

Player* Connect4::checkForWinner()
{
    if (!_grid) return nullptr;
    
    // Check horizontal, vertical, and both diagonals for 4 in a row
    
    // Check horizontal
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            Bit* b1 = _grid->getSquare(x, y)->bit();
            Bit* b2 = _grid->getSquare(x + 1, y)->bit();
            Bit* b3 = _grid->getSquare(x + 2, y)->bit();
            Bit* b4 = _grid->getSquare(x + 3, y)->bit();
            
            if (b1 && b2 && b3 && b4 && 
                b1->getOwner() == b2->getOwner() && 
                b2->getOwner() == b3->getOwner() && 
                b3->getOwner() == b4->getOwner()) {
                return b1->getOwner();
            }
        }
    }
    
    // Check vertical
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            Bit* b1 = _grid->getSquare(x, y)->bit();
            Bit* b2 = _grid->getSquare(x, y + 1)->bit();
            Bit* b3 = _grid->getSquare(x, y + 2)->bit();
            Bit* b4 = _grid->getSquare(x, y + 3)->bit();
            
            if (b1 && b2 && b3 && b4 && 
                b1->getOwner() == b2->getOwner() && 
                b2->getOwner() == b3->getOwner() && 
                b3->getOwner() == b4->getOwner()) {
                return b1->getOwner();
            }
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            Bit* b1 = _grid->getSquare(x, y)->bit();
            Bit* b2 = _grid->getSquare(x + 1, y + 1)->bit();
            Bit* b3 = _grid->getSquare(x + 2, y + 2)->bit();
            Bit* b4 = _grid->getSquare(x + 3, y + 3)->bit();
            
            if (b1 && b2 && b3 && b4 && 
                b1->getOwner() == b2->getOwner() && 
                b2->getOwner() == b3->getOwner() && 
                b3->getOwner() == b4->getOwner()) {
                return b1->getOwner();
            }
        }
    }
    
    // Check diagonal (top-right to bottom-left)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 3; x < CONNECT4_COLS; x++) {
            Bit* b1 = _grid->getSquare(x, y)->bit();
            Bit* b2 = _grid->getSquare(x - 1, y + 1)->bit();
            Bit* b3 = _grid->getSquare(x - 2, y + 2)->bit();
            Bit* b4 = _grid->getSquare(x - 3, y + 3)->bit();
            
            if (b1 && b2 && b3 && b4 && 
                b1->getOwner() == b2->getOwner() && 
                b2->getOwner() == b3->getOwner() && 
                b3->getOwner() == b4->getOwner()) {
                return b1->getOwner();
            }
        }
    }
    
    return nullptr;
}

bool Connect4::checkForDraw()
{
    if (!_grid) return true;
    
    // Check if board is full with no winner
    for (int x = 0; x < CONNECT4_COLS; x++) {
        ChessSquare* topSquare = _grid->getSquare(x, 0);
        if (topSquare && !topSquare->bit()) {
            return false; // Found an empty square, not a draw yet
        }
    }
    
    // Board is full
    return true;
}

std::string Connect4::initialStateString()
{
    std::string state = "";
    for (int i = 0; i < CONNECT4_ROWS * CONNECT4_COLS; i++)
    {
        state += "0";
    }
    return state;
}

std::string Connect4::stateString()
{
    std::string state = "";
    if (!_grid) return state;
    
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            ChessSquare* square = _grid->getSquare(x, y);
            if (square && square->bit()) {
                Player* owner = square->bit()->getOwner();
                if (owner == getPlayerAt(0)) {
                    state += "1";
                } else {
                    state += "2";
                }
            } else {
                state += "0";
            }
        }
    }
    return state;
}

void Connect4::setStateString(const std::string &s)
{
    if (!_grid) return;
    
    // Clear board
    stopGame();
    
    // Recreate board from state string
    int index = 0;
    for (int y = 0; y < CONNECT4_ROWS && index < s.length(); y++) {
        for (int x = 0; x < CONNECT4_COLS && index < s.length(); x++) {
            ChessSquare* square = _grid->getSquare(x, y);
            if (square) {
                char c = s[index];
                if (c == '1' || c == '2') {
                    int playerNum = (c == '1') ? 0 : 1;
                    Bit* piece = PieceForPlayer(playerNum);
                    piece->setPosition(square->getPosition());
                    square->setBit(piece);
                }
            }
            index++;
        }
    }
}

// AI Methods

bool Connect4::aiTestForTerminalState(std::string &state, Player *&winner)
{
    // Check if board is full
    if (state.find('0') == std::string::npos) {
        return true;
    }

    // Check for winner
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            int pos = y * CONNECT4_COLS;
            char b1 = state[pos + x];
            char b2 = state[pos + x + 1];
            char b3 = state[pos + x + 2];
            char b4 = state[pos + x + 3];
            if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
                winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
                return true;
            }
        }
    }

    // Check vertical
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos = (y + 1) * CONNECT4_COLS + x;
            int pos3 = (y + 2) * CONNECT4_COLS + x;
            int pos4 = (y + 3) * CONNECT4_COLS + x;
            char b1 = state[pos1];
            char b2 = state[pos];
            char b3 = state[pos3];
            char b4 = state[pos4];
            if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
                winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
                return true;
            }
        }
    }

    // Check diagonal (top-left to bottom-right)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos = (y + 1) * CONNECT4_COLS + (x + 1);
            int pos3 = (y + 2) * CONNECT4_COLS + (x + 2);
            int pos4 = (y + 3) * CONNECT4_COLS + (x + 3);
            char b1 = state[pos1];
            char b2 = state[pos];
            char b3 = state[pos3];
            char b4 = state[pos4];
            if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
                winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
                return true;
            }
        }
    }

    // Check diagonal (top-right to bottom-left)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 3; x < CONNECT4_COLS; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos = (y + 1) * CONNECT4_COLS + (x - 1);
            int pos3 = (y + 2) * CONNECT4_COLS + (x - 2);
            int pos4 = (y + 3) * CONNECT4_COLS + (x - 3);
            char b1 = state[pos1];
            char b2 = state[pos];
            char b3 = state[pos3];
            char b4 = state[pos4];
            if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
                winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
                return true;
            }
        }
    }

    return false;
}

void Connect4::setAIPlayer(int playerNumber, bool isAI) {
    if (playerNumber >= 0 && playerNumber < 2) {
        getPlayerAt(playerNumber)->setAIPlayer(isAI);
    }
}

bool Connect4::gameHasAI() {
    return getCurrentPlayer() && getCurrentPlayer()->isAIPlayer();
}

void Connect4::updateAI() {
    if (!gameHasAI() || !_grid) return;
    
    std::string state = stateString();
    int depth = 6;
    int bestScore = INT_MIN;
    _bestMoveColumn = 0;
    
    // Determine which player the AI is (1 for player 0, 2 for player 1)
    int aiPlayerNum = (getCurrentPlayer() == getPlayerAt(0)) ? 1 : 2;
    char aiChar = (aiPlayerNum == 1) ? '1' : '2';
    char opponentChar = (aiPlayerNum == 1) ? '2' : '1';

    // Try each column
    for (int col = 0; col < CONNECT4_COLS; col++) {
        // Check if column is full
        ChessSquare* topSquare = _grid->getSquare(col, 0);
        if (topSquare && topSquare->bit()) continue;

        // Make move in state string
        std::string testState = state;
        bool moved = false;
        for (int y = CONNECT4_ROWS - 1; y >= 0; y--) {
            int index = y * CONNECT4_COLS + col;
            if (index < testState.length() && testState[index] == '0') {
                testState[index] = aiChar;
                moved = true;
                break;
            }
        }

        if (!moved) continue;

        // Evaluate position
        int score = -negamax(testState, depth - 1, -1, aiChar, opponentChar);
        if (score > bestScore) {
            bestScore = score;
            _bestMoveColumn = col;
        }
    }
    
    // Actually make the move
    ChessSquare* targetCol = _grid->getSquare(_bestMoveColumn, 0);
    if (targetCol) {
        actionForEmptyHolder(*targetCol);
    }
}

int Connect4::negamax(std::string &state, int depth, int playerColor, char aiChar, char opponentChar) {
    Player* winner = nullptr;
    bool isTerminal = aiTestForTerminalState(state, winner);
    
    if (isTerminal) {
        if (!winner) return 0;
        return (winner->playerNumber() == (aiChar == '1' ? 0 : 1)) ? 1000 : -1000;
    }

    if (depth == 0) {
        return aiBoardEvaluation(state, aiChar, opponentChar);
    }

    int maxScore = INT_MIN;
    char currentPlayerChar = (playerColor == 1) ? aiChar : opponentChar;

    // Try each column
    for (int col = 0; col < CONNECT4_COLS; col++) {
        // Check if column is full
        if (state[col] != '0') continue;

        // Make move
        std::string newState = state;
        bool moved = false;
        for (int y = CONNECT4_ROWS - 1; y >= 0; y--) {
            int index = y * CONNECT4_COLS + col;
            if (newState[index] == '0') {
                newState[index] = currentPlayerChar;
                moved = true;
                break;
            }
        }

        if (!moved) continue;

        int score = -negamax(newState, depth - 1, -playerColor, aiChar, opponentChar);
        maxScore = (score > maxScore) ? score : maxScore;
    }

    return (maxScore == INT_MIN) ? 0 : maxScore;
}

int Connect4::aiBoardEvaluation(const std::string &state, char aiChar, char oppChar) {
    Player* winner = nullptr;
    std::string mutableState = state;
    
    if (aiTestForTerminalState(mutableState, winner)) {
        if (!winner) return 0;
        return (winner->playerNumber() == (aiChar == '1' ? 0 : 1)) ? 1000 : -1000;
    }

    int score = 0;
    
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 2; x++) {
            int i = y * CONNECT4_COLS;
            if (state[i + x] == aiChar && state[i + x + 1] == aiChar && state[i + x + 2] == aiChar) {
                score += 10;
            }
            if (state[i + x] == oppChar && state[i + x + 1] == oppChar && state[i + x + 2] == oppChar) {
                score -= 10;
            }
        }
    }
    
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 1; x++) {
            int i = y * CONNECT4_COLS;
            if (state[i + x] == aiChar && state[i + x + 1] == aiChar) {
                score += 3;
            }
            if (state[i + x] == oppChar && state[i + x + 1] == oppChar) {
                score -= 3;
            }
        }
    }
    
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        int centerpos = y * CONNECT4_COLS + (CONNECT4_COLS / 2);
        if (state[centerpos] == aiChar) {
            score += 2;
        } else if (state[centerpos] == oppChar) {
            score -= 2;
        }
    }
    
    return score;
}