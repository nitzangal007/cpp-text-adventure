#pragma once

#include <conio.h>
#include <vector>
#include <chrono>
#include "Player.h"
#include "Screens.h"
#include "Menu.h"
#include "Bomb.h"
#include "AutoBomb.h"
#include "Riddle.h"
#include "Room3Boss.h"

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
    ExitInfo exits[Screens::NUM_SCREENS-1];
    bool player1ReadyForNextScreen;
    bool player2ReadyForNextScreen;
    bool gameOver = false;

    // Lives & Score System
    int lives = 6;              // Starting lives
    int score = 0;              // Cumulative score
    int levelStartTime = 0;     // Timestamp when level started (in seconds)
    
    // M-Trap timer state
    std::chrono::steady_clock::time_point mTrapTimerStart;
    bool mTrapVisible = true;   // Current visibility state
    
    // Pause duration tracking (for freezing timers)
    std::chrono::steady_clock::time_point pauseStartTime;
    long long accumulatedPauseMs = 0;  // Total pause duration in ms
    long long accumulatedPauseSec = 0; // Total pause duration in seconds (for score)

    // Room 3 Boss
    Room3Boss room3Boss;

    // Story overlay flags (reset only on new game, not on death)
    bool shownStory1 = false;
    bool shownStory2 = false;
    
    // Iteration counter for steps/results recording (Exercise 3)
    size_t currentIteration_ = 0;

public:
    Game();
    virtual ~Game() = default;  // Virtual destructor for polymorphism

    // ==========================================
    // Public Interface
    // ==========================================

    // Main entry point: shows menu, handles loops
    void run();

protected:
    // ==========================================
    // Virtual Hooks for Polymorphism (Exercise 3)
    // ==========================================
    // These allow derived classes to customize behavior
    // without changing the core game loop.
    
    // Gets next keyboard input. Override to read from file instead.
    // Returns 0 if no input available this frame.
    virtual char getNextInput();
    
    // Called when a valid movement/action key is pressed.
    // Override to record steps to file.
    virtual void onInputReceived(size_t iteration, int playerId, char key) {}
    
    // Called when a significant game event occurs.
    // eventType: 0=ScreenTransition, 1=LifeLost, 2=Riddle, 3=GameFinished
    virtual void onResultEvent(size_t iteration, int playerId, int eventType, int extraData = 0) {}
    
    // Should rendering happen? Override to disable for silent mode.
    virtual bool shouldRender() const { return true; }
    
    // Sleep duration in ms. Override for faster replay.
    virtual int getSleepDuration() const;
    
    // Should menu be shown? Override to skip menu in load mode.
    virtual bool shouldShowMenu() const { return true; }

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

    bool handleRiddleEncounter(Player& player, const Point& nextPos);

    // ==========================================
    // M-Trap Logic
    // ==========================================
    
    // Update timer and check for visibility toggle
    void updateMTrapTimer();
    
    // Query current visibility state
    bool isMTrapVisible() const;
    
    // Check if player is standing on a visible M-trap (returns true if death)
    bool checkMTrapDeath(const Player& player) const;

    // ==========================================
    // Story Overlay System
    // ==========================================
    
    // Display a story message overlay (waits for keypress)
    void showStoryOverlay(int storyNumber);

};
