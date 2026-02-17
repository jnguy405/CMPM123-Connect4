#include "Connect4.h"
#include <climits>
#include <algorithm>

const int CONNECT4_COLS = 7;
const int CONNECT4_ROWS = 6;

// Move ordering - center columns first
const int COL_ORDER[7] = {3, 2, 4, 1, 5, 0, 6};

Connect4::Connect4() : Game()
{
    _grid = new Grid(CONNECT4_COLS, CONNECT4_ROWS);
    _bestMoveColumn = 0;
    _boardRed = 0;
    _boardYellow = 0;
    
    // Pre-compute the 4 shift patterns that cover all 69 winning lines
    _patterns[0] = {1, 2};   // Vertical (stride1=1, stride2=2)
    _patterns[1] = {7, 14};  // Horizontal (stride1=7, stride2=14)
    _patterns[2] = {8, 16};  // Diagonal down-right (stride1=8, stride2=16)
    _patterns[3] = {6, 12};  // Diagonal up-right (stride1=6, stride2=12)
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
    
    _grid->initializeSquares(80.0f, "square.png");

    startGame();
}

uint64_t Connect4::boardToBitboard(const std::string& state, char player) {
    uint64_t bitboard = 0;
    
    for (int i = 0; i < state.length() && i < 42; i++) {
        if (state[i] == player) {
            bitboard |= (1ULL << i);
        }
    }
    
    return bitboard;
}

bool Connect4::checkWinShift(uint64_t board) {
    uint64_t t;
    
    for (int i = 0; i < NUM_PATTERNS; i++) {
        t = board & (board >> _patterns[i].stride1);
        if (t & (t >> _patterns[i].stride2)) return true;
    }
    
    return false;
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {
    ChessSquare* square = static_cast<ChessSquare*>(&holder);
    if (!square) return false;

    int col = square->getColumn();
    
    int dropRow = -1;
    for (int y = CONNECT4_ROWS - 1; y >= 0; y--) {
        ChessSquare* target = _grid->getSquare(col, y);
        if (target && !target->bit()) {
            dropRow = y;
            break;
        }
    }

    if (dropRow == -1) return false;

    ChessSquare* dropSquare = _grid->getSquare(col, dropRow);
    if (!dropSquare) return false;

    int playerNum = (getCurrentPlayer() == getPlayerAt(0)) ? 0 : 1;
    Bit* piece = PieceForPlayer(playerNum);
    piece->setPosition(dropSquare->getPosition());
    dropSquare->setBit(piece);
    
    int pos = dropRow * CONNECT4_COLS + col;
    uint64_t bitPos = 1ULL << pos;
    
    if (playerNum == 0) {
        _boardRed |= bitPos;
    } else {
        _boardYellow |= bitPos;
    }

    endTurn();
    return true;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src) {
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    return false;
}

void Connect4::stopGame()
{
    if (_grid) {
        _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            if (square && square->bit()) {
                square->destroyBit();
            }
        });
    }
    _boardRed = 0;
    _boardYellow = 0;
}

Player* Connect4::checkForWinner()
{
    if (!_grid) return nullptr;
    
    std::string state = stateString();
    _boardRed = boardToBitboard(state, '1');
    _boardYellow = boardToBitboard(state, '2');
    
    if (checkWinShift(_boardRed)) {
        return getPlayerAt(0);
    }
    
    if (checkWinShift(_boardYellow)) {
        return getPlayerAt(1);
    }
    
    return nullptr;
}

bool Connect4::checkForDraw()
{
    if (!_grid) return true;
    
    for (int x = 0; x < CONNECT4_COLS; x++) {
        ChessSquare* topSquare = _grid->getSquare(x, 0);
        if (topSquare && !topSquare->bit()) {
            return false;
        }
    }
    
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
    
    stopGame();
    
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
                    
                    int pos = y * CONNECT4_COLS + x;
                    uint64_t bitPos = 1ULL << pos;
                    if (playerNum == 0) {
                        _boardRed |= bitPos;
                    } else {
                        _boardYellow |= bitPos;
                    }
                }
            }
            index++;
        }
    }
}

// -----------------------------------------------------------------------------
// AI Methods
// -----------------------------------------------------------------------------

bool Connect4::aiTestForTerminalState(std::string &state, Player *&winner)
{
    uint64_t redBoard = boardToBitboard(state, '1');
    uint64_t yellowBoard = boardToBitboard(state, '2');
    
    if (checkWinShift(redBoard)) {
        winner = getPlayerAt(0);
        return true;
    }
    
    if (checkWinShift(yellowBoard)) {
        winner = getPlayerAt(1);
        return true;
    }
    
    if (state.find('0') == std::string::npos) {
        return true;
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
    int depth = 8; // Can go deeper with optimizations!
    int bestScore = INT_MIN;
    _bestMoveColumn = 0;
    
    int aiPlayerNum = (getCurrentPlayer() == getPlayerAt(0)) ? 1 : 2;
    char aiChar = (aiPlayerNum == 1) ? '1' : '2';
    char opponentChar = (aiPlayerNum == 1) ? '2' : '1';

    // Try columns in optimized order (center first)
    for (int i = 0; i < CONNECT4_COLS; i++) {
        int col = COL_ORDER[i];
        
        // Check if column is full (check top row)
        if (state[0 * CONNECT4_COLS + col] != '0') continue;

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

        // Evaluate position with alpha-beta
        int score = -negamax(testState, depth - 1, INT_MIN + 1, INT_MAX, opponentChar, aiChar, opponentChar);
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

int Connect4::negamax(std::string &state, int depth, int alpha, int beta, char currentChar, char aiChar, char opponentChar) {
    Player* winner = nullptr;
    bool isTerminal = aiTestForTerminalState(state, winner);
    
    if (isTerminal) {
        if (!winner) return 0; // Draw
        
        bool aiWins = (winner->playerNumber() == (aiChar == '1' ? 0 : 1));
        return aiWins ? (1000 - depth) : (-1000 + depth);
    }

    if (depth == 0) {
        return aiBoardEvaluation(state, aiChar, opponentChar);
    }

    char nextChar = (currentChar == aiChar) ? opponentChar : aiChar;
    int maxScore = INT_MIN;

    // Try columns in optimized order
    for (int i = 0; i < CONNECT4_COLS; i++) {
        int col = COL_ORDER[i];
        
        // Check if column is full (top row occupied)
        if (state[0 * CONNECT4_COLS + col] != '0') continue;

        std::string newState = state;
        bool moved = false;
        for (int y = CONNECT4_ROWS - 1; y >= 0; y--) {
            int index = y * CONNECT4_COLS + col;
            if (newState[index] == '0') {
                newState[index] = currentChar;
                moved = true;
                break;
            }
        }

        if (!moved) continue;

        int score = -negamax(newState, depth - 1, -beta, -alpha, nextChar, aiChar, opponentChar);
        
        if (score > maxScore) {
            maxScore = score;
        }
        
        alpha = std::max(alpha, score);
        if (alpha >= beta) {
            break; // Alpha-beta pruning
        }
    }

    return (maxScore == INT_MIN) ? 0 : maxScore;
}

int Connect4::aiBoardEvaluation(const std::string &state, char aiChar, char oppChar) {
    // Convert to bitboards for faster evaluation
    uint64_t aiBoard = boardToBitboard(state, aiChar);
    uint64_t oppBoard = boardToBitboard(state, oppChar);
    
    int score = 0;
    
    // Center column preference (strategic advantage)
    for (int row = 0; row < CONNECT4_ROWS; row++) {
        int pos = row * CONNECT4_COLS + (CONNECT4_COLS / 2);
        uint64_t centerMask = 1ULL << pos;
        
        if (aiBoard & centerMask) {
            score += 3;
        } else if (oppBoard & centerMask) {
            score -= 3;
        }
    }
    
    // Check for 3-in-a-row threats in all directions using bit shifts
    uint64_t t;
    
    // Horizontal threats
    t = aiBoard & (aiBoard >> 1);
    if (t & (aiBoard >> 2)) score += 50; // AI has 3 in a row horizontally
    
    t = oppBoard & (oppBoard >> 1);
    if (t & (oppBoard >> 2)) score -= 50; // Opponent has 3 in a row horizontally
    
    // Vertical threats
    t = aiBoard & (aiBoard >> CONNECT4_COLS);
    if (t & (aiBoard >> (2 * CONNECT4_COLS))) score += 50;
    
    t = oppBoard & (oppBoard >> CONNECT4_COLS);
    if (t & (oppBoard >> (2 * CONNECT4_COLS))) score -= 50;
    
    // Diagonal down-right threats
    t = aiBoard & (aiBoard >> (CONNECT4_COLS + 1));
    if (t & (aiBoard >> (2 * (CONNECT4_COLS + 1)))) score += 50;
    
    t = oppBoard & (oppBoard >> (CONNECT4_COLS + 1));
    if (t & (oppBoard >> (2 * (CONNECT4_COLS + 1)))) score -= 50;
    
    // Diagonal up-right threats
    t = aiBoard & (aiBoard >> (CONNECT4_COLS - 1));
    if (t & (aiBoard >> (2 * (CONNECT4_COLS - 1)))) score += 50;
    
    t = oppBoard & (oppBoard >> (CONNECT4_COLS - 1));
    if (t & (oppBoard >> (2 * (CONNECT4_COLS - 1)))) score -= 50;
    
    return score;
}