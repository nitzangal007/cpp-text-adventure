#pragma once

#include "Point.h"
#include "Player.h"
#include "GameConstants.h"
#include <random>
#include <chrono>
#include <string>
#include <cstdint>
//
//We used chatGpt to help us design the state machine and overall structure of this class. 
//

class Screens;  // Forward declaration

// ==========================================
// Boss State Machine
// ==========================================

enum class BossState {
    Inactive,       // Not Room 3, or boss not reached yet
    PreBoss,        // Waiting for H briefing or start switch
    Briefing,       // H overlay shown, game frozen
    Countdown,      // 3-2-1 countdown after start switch
    TaskRunning,    // Active task, timer running
    TaskFail,       // Explosion/reset (brief state)
    TaskSuccess,    // Brief success feedback
    Victory,        // All tasks complete, M removed, safe mode
    Exit            // Both players on '3', level complete
};

// ==========================================
// Task Data Structure
// ==========================================

struct TaskData {
    int taskNumber = 0;         // 1, 2, or 3
    int timeLimit = 0;          // 20, 40, or 60 seconds
    uint8_t targetValue = 0;    // The correct answer
    uint8_t operandA = 0;       // For tasks 2-3
    uint8_t operandB = 0;       // For tasks 2-3
    std::string displayText;    // Task instruction text
};

// ==========================================
// Boss Switch Structure
// ==========================================

struct BossSwitch {
    Point position;
    bool isOn = false;
    bool wasPlayerOnLastFrame = false;  // For edge detection
};

// ==========================================
// Room3Boss Class
// ==========================================

class Room3Boss {
public:
    Room3Boss();

    // ==========================================
    // Core Lifecycle
    // ==========================================
    
    // Initialize boss for Room 3 (called when entering Room 3)
    void init();
    
    // Full reset (called on game restart)
    void reset();

    // ==========================================
    // Frame Update & Input
    // ==========================================
    
    // Called every frame from Game::updateLogic() when on Room 3
    void update(Screens& screens, Player& p1, Player& p2);
    
    // Handle input (returns true if input was consumed by boss)
    bool handleInput(char key, Screens& screens, Player& p1, Player& p2);

    // ==========================================
    // Rendering
    // ==========================================
    
    // Draw full-screen overlays (countdown only now)
    void drawOverlay() const;
    
    // Draw task bar in legend area (task text, timer, bits)
    void drawTaskBar(int legendY) const;
    
    // Show briefing (BLOCKING - draws once, waits for key, then returns)
    void showBriefing();

    // ==========================================
    // State Queries
    // ==========================================
    
    BossState getState() const { return state_; }
    bool isActive() const { return state_ != BossState::Inactive; }
    bool isVictory() const { return state_ == BossState::Victory || state_ == BossState::Exit; }
    bool isFrozen() const { return state_ == BossState::Briefing; }
    
    // Bomb blinking (last 10 seconds of task)
    bool shouldBombsBlink() const;
    bool areBombsCurrentlyVisible() const;

    // ==========================================
    // Failure Callback
    // ==========================================
    
    // Called by Game when player steps on 'B' tile during TaskRunning
    void onBombStepped(Screens& screens, Player& p1, Player& p2);

    // ==========================================
    // Penalty System
    // ==========================================
    
    int getScorePenalty() const { return pendingScorePenalty_; }
    int getLifePenalty() const { return pendingLifePenalty_; }
    void clearPenalties() { pendingScorePenalty_ = 0; pendingLifePenalty_ = 0; }
    
    // Check if 'R' key should be disabled
    bool isRestartDisabled() const;

private:
    // ==========================================
    // State Machine
    // ==========================================
    
    BossState state_ = BossState::Inactive;
    void transitionTo(BossState newState);

    // ==========================================
    // Task Tracking
    // ==========================================
    
    int currentTaskIndex_ = 0;  // 0, 1, 2 for tasks 1-3
    TaskData taskData_;
    bool taskValuesGenerated_ = false;
    
    // Timing
    std::chrono::steady_clock::time_point taskStartTime_;
    std::chrono::steady_clock::time_point countdownStartTime_;
    int countdownValue_ = 3;
    
    // Briefing shown flag (prevents re-trigger while on H tile)
    bool briefingShown_ = false;

    // ==========================================
    // 8-Bit Switch System
    // ==========================================
    
    BossSwitch switches_[8];  // 0-3 = left (bits 7-4), 4-7 = right (bits 3-0)
    void initSwitchPositions();
    void updateSwitches(Screens& screens, const Player& p1, const Player& p2);
    uint8_t readSwitchByte() const;
    std::string getBitsString() const;

    // ==========================================
    // Board Snapshot / Restore
    // ==========================================
    
    char boardSnapshot_[Screen::MAX_Y][Screen::MAX_X];
    Point player1Snapshot_;
    Point player2Snapshot_;
    bool switchStatesSnapshot_[8];
    
    void takeSnapshot(const Screens& screens, const Player& p1, const Player& p2);
    void restoreSnapshot(Screens& screens, Player& p1, Player& p2);

    // ==========================================
    // Random Number Generator
    // ==========================================
    
    std::mt19937 rng_;
    void generateTaskValues();

    // ==========================================
    // Task Flow
    // ==========================================
    
    void startTask(Screens& screens, Player& p1, Player& p2);
    void handleTaskSuccess(Screens& screens);
    void handleTaskFailure(Screens& screens, Player& p1, Player& p2);
    int getTimeRemaining() const;

    // ==========================================
    // Victory & Cleanup
    // ==========================================
    
    void removeExitBlocker(Screens& screens);
    void removeAllBombs(Screens& screens);
    void closeEntrances(Screens& screens);
    void removeTorchesFromPlayers(Player& p1, Player& p2);

    // ==========================================
    // Bomb Blinking
    // ==========================================
    
    std::chrono::steady_clock::time_point blinkStartTime_;
    bool bombsVisible_ = true;
    void updateBombBlink();

    // ==========================================
    // Penalties
    // ==========================================
    
    int pendingScorePenalty_ = 0;
    int pendingLifePenalty_ = 0;

    // ==========================================
    // Key Coordinates (Y, X format)
    // ==========================================
    
    static constexpr int H_TILE_Y = 15;
    static constexpr int H_TILE_X = 37;
    
    static constexpr int START_SWITCH_Y = 15;
    static constexpr int START_SWITCH_X = 41;
    
    static constexpr int LEFT_ENTRANCE_Y = 12;
    static constexpr int LEFT_ENTRANCE_X = 25;
    
    static constexpr int RIGHT_ENTRANCE_Y = 12;
    static constexpr int RIGHT_ENTRANCE_X = 37;
    
    static constexpr int EXIT_3_Y = 20;
    static constexpr int EXIT_3_X = 41;
    
    static constexpr int M_BLOCKER_Y = 19;
    static constexpr int M_BLOCKER_X = 41;

    // ==========================================
    // Task Constants
    // ==========================================
    
    static constexpr int TASK1_TIME = 20;
    static constexpr int TASK2_TIME = 40;
    static constexpr int TASK3_TIME = 60;
    static constexpr int BOMB_BLINK_THRESHOLD = 10;  // Last 10 seconds
    static constexpr int BOMB_BLINK_INTERVAL_MS = 500;
    static constexpr int SCORE_PENALTY = 500;
    static constexpr int LIFE_PENALTY = 2;
};
