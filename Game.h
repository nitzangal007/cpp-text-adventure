#pragma once

#include <conio.h>
#include <vector>
#include "Player.h"
#include "Screens.h"
#include "Menu.h"
#include "Bomb.h"
#include "AutoBomb.h"

// What should happen when the game loop exits
enum class GameResult { BackToMenu, QuitProgram };



class Game
{
    // ==========================================
    // Internal Structures
    // ==========================================
    struct ExitInfo {
        Screens::ScreenId from;
        Screens::ScreenId to;
        Point doorPos;
        Point waitPos;
        Point nextStartP1;
        Point nextStartP2;
    };

    // ==========================================
    // Member Variables
    // ==========================================
    Screens currentScreen;
    Player  player1;
    Player  player2;
    Point   player1Start;
    Point   player2Start;

    // Bomb state (now using item classes)
    Bomb bomb;
    std::vector<AutoBomb> autoBombs;

    // Navigation
    ExitInfo exits[Screens::NUM_SCREENS];
    bool player1ReadyForNextScreen;
    bool player2ReadyForNextScreen;
    bool gameOver = false;

    // Lives & Score System
    int lives = 6;              // Starting lives
    int score = 0;              // Cumulative score
    int levelStartTime = 0;     // Timestamp when level started (in seconds)

public:
    Game();

    // ==========================================
    // Public Interface
    // ==========================================

    // Main entry point: shows menu, handles loops
    void run();

private:
    // ==========================================
    // Core Game Control
    // ==========================================

    // Initialize/Reset game state
    void initGame();
    
    // Main loop for a single game session
    void runGame();
    
    // Reinits current level (e.g. after death)
    void resetCurrentGame();

    // ==========================================
    // Update & Render
    // ==========================================

    // Frame logic: movement, switches, bombs
    void updateLogic();

    // Draw map, players, UI
    void render();
    void drawStatusBar();

    // ==========================================
    // Player Logic
    // ==========================================

    void updatePlayerMovement(Player& player);
    void collectItemIfPossible(Player& player);
    Player& getOtherPlayer(const Player& p);

    // ==========================================
    // Bomb Logic
    // ==========================================

    void tryPlaceBomb(Player& player);
    void explodeBomb();
    
    // Handle auto-bombs (returns true if player died)
    bool handleAutoBombs();

    bool isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared) const;

    // ==========================================
    // Torch Logic
    // ==========================================
    
    // Drop torch back onto the board
    void dropTorch(Player& player);

    // ==========================================
    // Spring Logic
    // ==========================================
    
    // Checks release conditions and triggers launch if met
    void updateSpringLogic(Player& player);
    // Handles forced movement during Launching mode
    void processForcedMove(Player& player, Player& otherPlayer);
    // Helper: Check if position is blocked for flight
    bool isBlockedForFlight(const Point& pos) const;
    // Helper: Check if two directions are perpendicular
    bool isPerpendicular(Direction d1, Direction d2) const;
    // Helper: Get direction from player's current movement delta
    Direction getPlayerInputDirection(const Player& player) const;

    // ==========================================
    // Screen Transition Logic
    // ==========================================

    // Checks readiness and moves to next screen if OK
    void tryAdvanceToNextScreen();

    bool playerIsReadyForNextScreen(const Player& player) const;
    bool isExitWaitPosition(const Point& p) const;

    // ==========================================
    // Lives & Score System
    // ==========================================

    // Decrements life and triggers game over if lives reach 0
    void decrementLife();

    // Calculates and adds score when completing a level
    void addLevelCompletionScore();

    // Displays game over screen with final score
    void showGameOverScreen();

    // Gets current time in seconds since epoch
    int getCurrentTimeSeconds() const;
};
