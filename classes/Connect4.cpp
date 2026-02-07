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

// Check for winner helper function
Player* Connect4::isWinner(ChessSquare* s1, ChessSquare* s2, ChessSquare* s3, ChessSquare* s4)
{
    if (s1 && s2 && s3 && s4) {
        Bit* b1 = s1->bit();
        Bit* b2 = s2->bit();
        Bit* b3 = s3->bit();
        Bit* b4 = s4->bit();

        if (b1 && b2 && b3 && b4 &&
            b1->getOwner() == b2->getOwner() &&
            b2->getOwner() == b3->getOwner() &&
            b3->getOwner() == b4->getOwner()) {
            return b1->getOwner();
        }
    }
    return nullptr;
}

Player* Connect4::checkForWinner()
{
    if (!_grid) return nullptr;
    
    // Check horizontal, vertical, and both diagonals for 4 in a row
    // Other ways:
    // Bit board
    // marching approach using Grid directional helpers

    // Check horizontal (using getE)
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            ChessSquare* s1 = _grid->getSquare(x, y);
            ChessSquare* s2 = s1 ? _grid->getE(x, y) : nullptr;
            ChessSquare* s3 = s2 ? _grid->getE(s2->getColumn(), s2->getRow()) : nullptr;
            ChessSquare* s4 = s3 ? _grid->getE(s3->getColumn(), s3->getRow()) : nullptr;
            
            if (isWinner(s1, s2, s3, s4)) {
                return s1->bit()->getOwner();
            }
        }
    }
    
    // Check vertical (using getS)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            ChessSquare* s1 = _grid->getSquare(x, y);
            ChessSquare* s2 = s1 ? _grid->getS(x, y) : nullptr;
            ChessSquare* s3 = s2 ? _grid->getS(s2->getColumn(), s2->getRow()) : nullptr;
            ChessSquare* s4 = s3 ? _grid->getS(s3->getColumn(), s3->getRow()) : nullptr;
            
            if (isWinner(s1, s2, s3, s4)) {
                return s1->bit()->getOwner();
            }
        }
    }
    
    // Check diagonal (using getBR: top left -> bottom-right)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            ChessSquare* s1 = _grid->getSquare(x, y);
            ChessSquare* s2 = s1 ? _grid->getBR(x, y) : nullptr;
            ChessSquare* s3 = s2 ? _grid->getBR(s2->getColumn(), s2->getRow()) : nullptr;
            ChessSquare* s4 = s3 ? _grid->getBR(s3->getColumn(), s3->getRow()) : nullptr;
            
            if (isWinner(s1, s2, s3, s4)) {
                return s1->bit()->getOwner();
            }
        }
    }
    
    // Check diagonal (using getBL: top-right -> bottom-left)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 3; x < CONNECT4_COLS; x++) {
            ChessSquare* s1 = _grid->getSquare(x, y);
            ChessSquare* s2 = s1 ? _grid->getBL(x, y) : nullptr;
            ChessSquare* s3 = s2 ? _grid->getBL(s2->getColumn(), s2->getRow()) : nullptr;
            ChessSquare* s4 = s3 ? _grid->getBL(s3->getColumn(), s3->getRow()) : nullptr;
            
            if (isWinner(s1, s2, s3, s4)) {
                return s1->bit()->getOwner();
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
    
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
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
    });
    
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

// -----------------------------------------------------------------------------
// AI Methods
// -----------------------------------------------------------------------------

// AI helper: check if 4 positions form a winning line in state string
bool Connect4::aiIsWinner(const std::string &state, int pos1, int pos2, int pos3, int pos4, Player *&winner)
{
    if (pos1 >= 0 && pos1 < state.length() &&
        pos2 >= 0 && pos2 < state.length() &&
        pos3 >= 0 && pos3 < state.length() &&
        pos4 >= 0 && pos4 < state.length()) {
        
        char b1 = state[pos1];
        char b2 = state[pos2];
        char b3 = state[pos3];
        char b4 = state[pos4];
        
        if (b1 != '0' && b1 == b2 && b2 == b3 && b3 == b4) {
            winner = (b1 == '1') ? getPlayerAt(0) : getPlayerAt(1);
            return true;
        }
    }
    return false;
}

bool Connect4::aiTestForTerminalState(std::string &state, Player *&winner)
{
    // Check if board is full
    if (state.find('0') == std::string::npos) {
        return true;
    }

    // Check horizontal (right)
    for (int y = 0; y < CONNECT4_ROWS; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos2 = pos1 + 1;
            int pos3 = pos1 + 2;
            int pos4 = pos1 + 3;
            if (aiIsWinner(state, pos1, pos2, pos3, pos4, winner)) {
                return true;
            }
        }
    }

    // Check vertical (down)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos2 = pos1 + CONNECT4_COLS;
            int pos3 = pos2 + CONNECT4_COLS;
            int pos4 = pos3 + CONNECT4_COLS;
            if (aiIsWinner(state, pos1, pos2, pos3, pos4, winner)) {
                return true;
            }
        }
    }

    // Check diagonal (top-left -> bottom-right)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 0; x < CONNECT4_COLS - 3; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos2 = pos1 + CONNECT4_COLS + 1;
            int pos3 = pos2 + CONNECT4_COLS + 1;
            int pos4 = pos3 + CONNECT4_COLS + 1;
            if (aiIsWinner(state, pos1, pos2, pos3, pos4, winner)) {
                return true;
            }
        }
    }

    // Check diagonal (top-right -> bottom-left)
    for (int y = 0; y < CONNECT4_ROWS - 3; y++) {
        for (int x = 3; x < CONNECT4_COLS; x++) {
            int pos1 = y * CONNECT4_COLS + x;
            int pos2 = pos1 + CONNECT4_COLS - 1;
            int pos3 = pos2 + CONNECT4_COLS - 1;
            int pos4 = pos3 + CONNECT4_COLS - 1;
            if (aiIsWinner(state, pos1, pos2, pos3, pos4, winner)) {
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