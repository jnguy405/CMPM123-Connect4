#include "Connect4.h"

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
    
    // Initialize all squares with proper positioning - all squares enabled (like TicTacToe, not checkerboard)
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
    // Pieces in Connect4 can't be picked up and moved; moves are through actionForEmptyHolder only
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    // Pieces in Connect4 can't be picked up and moved; moves are through actionForEmptyHolder only
    return false;
}

void Connect4::stopGame()
{
    // Clear the board - loop through and destroy all bits
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (square && square->bit()) {
            square->destroyBit();
        }
    });
}

Player* Connect4::checkForWinner()
{
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
    // Check if board is full with no winner
    for (int x = 0; x < CONNECT4_COLS; x++) {
        ChessSquare* topSquare = _grid->getSquare(x, 0);
        if (topSquare && !topSquare->bit()) {
            return false; // Found an empty square, not a draw yet
        }
    }
    
    // Board is full and no winner found (checked before this is called)
    return true;
}

std::string Connect4::initialStateString()
{
    std::string state = "";
    for (int i = 0; i < CONNECT4_ROWS * CONNECT4_COLS; i++)
    {
        state += "0"; // Initialize each position as empty (represented by '0')
    }
    return state;
}

std::string Connect4::stateString()
{
    std::string state = "";
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            ChessSquare* square = _grid->getSquare(x, y);
            if (square && square->bit()) {
                // '1' for player 0 (red), '2' for player 1 (yellow)
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

void Connect4::updateAI()
{
    // Simple AI: use negamax to find best move
    std::string state = stateString();
    int depth = 6; // Search depth
    int bestScore = INT_MIN;
    _bestMoveColumn = 0;

    // Try each column
    for (int col = 0; col < CONNECT4_COLS; col++) {
        // Check if column is full
        ChessSquare* topSquare = _grid->getSquare(col, 0);
        if (topSquare && topSquare->bit()) continue; // Column full

        // Make move in state string
        std::string testState = state;
        bool moved = false;
        for (int y = CONNECT4_ROWS - 1; y >= 0; y--) {
            int index = y * CONNECT4_COLS + col;
            if (index < testState.length() && testState[index] == '0') {
                testState[index] = '2'; // AI is player 2 (yellow)
                moved = true;
                break;
            }
        }

        if (!moved) continue;

        // Evaluate position
        int score = -negamax(testState, depth - 1, -1); // Switch to opponent perspective
        if (score > bestScore) {
            bestScore = score;
            _bestMoveColumn = col;
        }
    }
}

bool Connect4::gameHasAI()
{
    // Check if current player is AI (player 2 = yellow = AI)
    return getCurrentPlayer() == getPlayerAt(1);
}

bool Connect4::aiTestForTerminalState(std::string &state, Player *&winner)
{
    // Check if board is full
    if (state.find('0') == std::string::npos) {
        return true; // Terminal state: draw
    }

    // Check for winner by analyzing state string
    // Reconstruct board piece positions and check
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            int idx = y * CONNECT4_COLS;
            char b1 = state[idx + x];
            char b2 = state[idx + x + 1];
            char b3 = state[idx + x + 2];
            char b4 = state[idx + x + 3];
            if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
                winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
                return true;
            }
        }
    }

    // Check vertical
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            int idx1 = y * CONNECT4_COLS + x;
            int idx2 = (y + 1) * CONNECT4_COLS + x;
            int idx3 = (y + 2) * CONNECT4_COLS + x;
            int idx4 = (y + 3) * CONNECT4_COLS + x;
            char b1 = state[idx1];
            char b2 = state[idx2];
            char b3 = state[idx3];
            char b4 = state[idx4];
            if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
                winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
                return true;
            }
        }
    }

    // Check diagonals (similar pattern)
    return false; // Not terminal
}

int Connect4::aiBoardEvaluation(const std::string& state)
{
    Player* winner = nullptr;
    std::string mutableState = state;
    
    if (aiTestForTerminalState(mutableState, winner)) {
        if (!winner) return 0; // Draw
        return (winner == getPlayerAt(1)) ? 1000 : -1000; // AI or opponent win
    }

    // Simple heuristic: count potential threats
    int score = 0;
    
    // Check for 3-in-a-row patterns (dangerous positions)
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 2; x++) {
            int idx = y * CONNECT4_COLS;
            if (state[idx + x] == '2' && state[idx + x + 1] == '2' && state[idx + x + 2] == '2') {
                score += 10; // AI has 3-in-a-row
            }
            if (state[idx + x] == '1' && state[idx + x + 1] == '1' && state[idx + x + 2] == '1') {
                score -= 10; // Opponent has 3-in-a-row
            }
        }
    }
    
    return score;
}

int Connect4::negamax(std::string& state, int depth, int playerColor)
{
    Player* winner = nullptr;
    bool isTerminal = aiTestForTerminalState(state, winner);
    
    if (isTerminal) {
        if (!winner) return 0; // Draw
        return (playerColor == 1) ? 1000 : -1000;
    }

    if (depth == 0) {
        return aiBoardEvaluation(state);
    }

    int maxScore = INT_MIN;

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
                newState[index] = (playerColor == 1) ? '2' : '1';
                moved = true;
                break;
            }
        }

        if (!moved) continue;

        int score = -negamax(newState, depth - 1, -playerColor);
        maxScore = (score > maxScore) ? score : maxScore;
    }

    return (maxScore == INT_MIN) ? 0 : maxScore;
}