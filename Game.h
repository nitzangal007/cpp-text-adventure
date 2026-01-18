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
#include "Steps.h"
#include "Results.h"

// What should happen when the game loop exits
enum class GameResult { BackToMenu, QuitProgram };

// Game mode for polymorphism
enum class GameMode { 
    Normal,      // Regular game (no save/load)
    Save,        // Save mode - record steps and results
    Load,        // Load mode - play from file
    LoadSilent   // Load silent - no display, just verify
};

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

protected:
    bool gameOver = false;  // Accessible to derived classes

private:

    // Lives & Score System
    int lives = 6;              // Starting lives
    int score = 0;              // Cumulative score
    int levelStartTime = 0;     // Timestamp when level started (in seconds)
    
    // M-Trap timer state
    std::chrono::steady_clock::time_point mTrapTimerStart;
    bool mTrapVisible = true;   // Current visibility state
    
    // Pause duration tracking (deprecated with new gameTimeMs system, but kept if needed for other logic)
    std::chrono::steady_clock::time_point pauseStartTime;
    long long accumulatedPauseMs = 0;  // Total pause duration in ms
    long long accumulatedPauseSec = 0; // Total pause duration in seconds (for score)

    // Deterministic Game Time (accumulates per tick, regardless of real execution speed)
    long long gameTimeMs = 0;

    // Room 3 Boss
    Room3Boss room3Boss;

    // Story overlay flags (reset only on new game, not on death)
    bool shownStory1 = false;
    bool shownStory2 = false;

protected:
    // ==========================================
    // Polymorphism Support (for save/load modes)
    // ==========================================
    
    // Game iteration counter (for save/load synchronization)
    size_t iteration = 0;
    
    // Steps and Results for save/load
    Steps steps;
    Results results;
    
    // ==========================================
    // Virtual Functions (override in derived classes)
    // ==========================================
    
    // Check if there's input available (keyboard or file)
    virtual bool hasInput();
    
    // Get the next input character
    virtual char getNextInput();
    
    // Render the game (can be empty in silent mode)
    virtual void doRender();
    
    // Get sleep duration in milliseconds
    virtual int getSleepDuration() const;
    
    // Check if we should show story overlays
    virtual bool shouldShowOverlay() const;
    
    // Record a step (save mode only)
    virtual void recordStep(char p1Key, char p2Key);
    
    // Record a result event
    virtual void recordLostLife();
    virtual void recordScreenChange(int screenNumber);
    virtual void recordRiddle(const std::string& question, const std::string& answer, bool correct);
    virtual void recordGameEnded(int finalScore);
    virtual void recordBossTaskComplete(int taskNumber);
    
    // Check if game should finish (e.g. end of replay in load mode)
    virtual bool shouldFinishGame() const;

    // Virtual function to handle riddle solving (interactive vs from file)
    virtual bool solveRiddle(class Riddle* r);

public:
    Game();
    virtual ~Game() = default;

    // ==========================================
    // Public Interface
    // ==========================================

    // Main entry point: shows menu, handles loops
    void run();

protected:
    // ==========================================
    // Core Game Control (accessible to derived classes)
    // ==========================================

    // Initialize/Reset game state
    void initGame();
    
    // Main loop for a single game session
    void runGame();
    
    // Reinits current level (e.g. after death)
    void resetCurrentGame();
    
    // Draw map, players, UI (accessible to derived classes for doRender override)
    void render();

private:

    // ==========================================
    // Update & Render
    // ==========================================

    // Frame logic: movement, switches, bombs
    void updateLogic();

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
