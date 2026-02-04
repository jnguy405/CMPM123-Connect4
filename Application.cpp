#include "Application.h"
#include "Logger.h"
#include "Command.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "classes/Connect4.h"

namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        // "Game Control" Window defaults
        static int gameActCounter = 0;                              // Track player actions by count
        static float floatVal = 0.0f;                               // Displays designated float val on slider
        static ImVec4 clearColor = ImVec4(0.5f, 0.5f, 0.5f, 1.00f); // Hex picker default

        static bool LogWin = true;
        static bool GameWin = true;      // TicTacToe game window
        static bool ControlWin = true;   // Game control panel

        // Default hex color (clear)
        static float colorR = 115.0f / 255.0f;
        static float colorG = 140.0f / 255.0f;
        static float colorB = 153.0f / 255.0f;

        static char InputBuf[256] = "";

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = nullptr;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                ImGui::Begin("Settings");

                if (gameOver) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Winner: %d", gameWinner);
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                }
                if (!game) {
                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Connect 4")) {
                        game = new Connect4();
                        game->setUpBoard();
                    }
                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    if (game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    game->drawFrame();
                }
                ImGui::End();

                if (LogWin) {
            ImGui::Begin("Game Log", &LogWin);

            // Filter state variables
            static bool showInfo = true;
            static bool showWarning = true;
            static bool showError = true;
            static bool showScores = false;

            // Options button and popup
            if (ImGui::Button("Options")) {
                ImGui::OpenPopup("OptionsPopup");
            }

            if (ImGui::BeginPopup("OptionsPopup")) {
                ImGui::Text("Filter Options");
                ImGui::Separator();
                
                ImGui::Checkbox("Show Info", &showInfo);
                ImGui::Checkbox("Show Warnings", &showWarning);
                ImGui::Checkbox("Show Errors", &showError);
                ImGui::Checkbox("Show Scores", &showScores);
                
                ImGui::Separator();
                
                if (ImGui::MenuItem("Clear Log")) {
                    Logger::GetInstance().Clear();
                }
                
                ImGui::EndPopup();
            }

                // Test buttons
                ImGui::SameLine();
                if (ImGui::Button("Clear")) {
                    Logger::GetInstance().Clear();
                }
                ImGui::SameLine();
                if (ImGui::Button("Test Info")) {
                    LOG_INFO("This is a test info message");
                }
                ImGui::SameLine();
                if (ImGui::Button("Test Warning")) {
                    LOG_WARN("This is a test warning message");
                }
                ImGui::SameLine();
                if (ImGui::Button("Test Error")) {
                    LOG_ERROR("This is a test error message");
                }
                ImGui::Separator();

                // Display log entries with filtering
                const auto& entries = Logger::GetInstance().GetEntries();
                const auto& colors = Logger::GetInstance().GetColors();
                
                const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
                ImGui::BeginChild("LogScrollRegion", ImVec2(0, -footer_height), true);
                
                for (size_t i = 0; i < entries.size(); i++) {
                    bool display = true;
                    
                    if (entries[i].find("[INFO]") != std::string::npos && !showInfo) { 
                        display = false; 
                    }
                    else if (entries[i].find("[WARN]") != std::string::npos && !showWarning) { 
                        display = false; 
                    }
                    else if (entries[i].find("[ERROR]") != std::string::npos && !showError) { 
                        display = false; 
                    }
                    else if (entries[i].find("[AI SCORE]") != std::string::npos && !showScores) { 
                        display = false; 
                    }
                    
                    if (display) {
                        ImGui::PushStyleColor(ImGuiCol_Text, colors[i]);
                        ImGui::Text("%s", entries[i].c_str());
                        ImGui::PopStyleColor();
                    }
                }
                
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    ImGui::SetScrollHereY(1.0f);
                }
                ImGui::EndChild();

                // Command line input
                ImGui::Separator();
                bool reclaim_focus = false;

                ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | 
                                                    ImGuiInputTextFlags_EscapeClearsAll | 
                                                    ImGuiInputTextFlags_CallbackHistory;
                
                if (ImGui::InputText("##CommandInput", InputBuf, IM_ARRAYSIZE(InputBuf), 
                                    input_text_flags, &Command::TextEditCallbackStub)) {
                    char* s = InputBuf;
                    Command::Strtrim(s);
                    if (s[0])
                        Command::ExecCommand(s);
                    strcpy_s(s, IM_ARRAYSIZE(InputBuf), "");
                    reclaim_focus = true;
                }
                
                ImGui::SetItemDefaultFocus();
                if (reclaim_focus)
                    ImGui::SetKeyboardFocusHere(-1);

                ImGui::SameLine();
                ImGui::Text("Command");

                ImGui::SameLine();
                if (ImGui::Button("Help")) {
                    LOG_INFO_TAG("Available commands: CLEAR, HELP, INFO, WARN, ERROR, RESET", "CMD");
                }

                ImGui::End();
            }

            // Window #3 - Game Control Panel
            if (ControlWin) {
                ImGui::Begin("Game Control", &ControlWin);
                ImGui::Text("Main Game Control Panel");

                // Window toggles
                ImGui::Checkbox("##LogCheck", &LogWin);
                ImGui::SameLine();
                ImGui::Text("Log Window");

                ImGui::Checkbox("##GameCheck", &GameWin);
                ImGui::SameLine();
                ImGui::Text("Game Window");

                ImGui::Separator();

                // Game control buttons
                ImGui::Text("Controls:");
                
                if (ImGui::Button("Reset Game")) {
                    if (game) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                        LOG_INFO("Game reset from control panel");
                    }
                }

                ImGui::Separator();
                // Visual controls
                ImGui::SliderFloat("##float", &floatVal, 0.0f, 1.0f, "%.3f");
                ImGui::SameLine();
                ImGui::Text("Test Slider");

                // Color edit control
                float colorArray[3] = { colorR, colorG, colorB };
                if (ImGui::ColorEdit3("Background Color", colorArray)) {
                    colorR = colorArray[0];
                    colorG = colorArray[1];
                    colorB = colorArray[2];
                    clearColor = ImVec4(colorR, colorG, colorB, 1.0f);
                }

                // Action counter
                if (ImGui::Button("Game Action")) {
                    gameActCounter++;
                    std::string msg = "Game action performed, counter: " + std::to_string(gameActCounter);
                    LOG_INFO_TAG(msg, "GAME");
                }
                ImGui::SameLine();
                ImGui::Text("Action Counter: %d", gameActCounter);

                ImGui::Separator();
                ImGui::Text("Logging Test Buttons:");

                if (ImGui::Button("Log Game Event")) {
                    LOG_INFO_TAG("Manual game event from control panel", "GAME");
                }
                ImGui::SameLine();
                if (ImGui::Button("Log Warnings")) {
                    LOG_WARN("Manual warning from control panel");
                }
                ImGui::SameLine();
                if (ImGui::Button("Log Error")) {
                    LOG_ERROR("Manual error from control panel");
                }

                ImGui::End();
            }
        }

    //
    // end turn is called by the game code at the end of each turn
    // this is where we check for a winner
    //
    void EndOfTurn() {
        if (!game) return;
        
        // Check for winner or draw
        Player *winner = game->checkForWinner();
        if (winner) {
            gameOver = true;
            gameWinner = winner->playerNumber() + 1;
            LOG_INFO_TAG("Game Over! Winner: Player " + std::to_string(gameWinner), "GAME");
        } else if (game->checkForDraw()) {
            gameOver = true;
            gameWinner = -1;
            LOG_INFO_TAG("Game Over! It's a draw.", "GAME");
        }
        
        // Increment action counter and log
        gameActCounter++;
    }
}