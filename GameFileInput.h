#pragma once

#include "Game.h"
#include "Steps.h"
#include "Results.h"

// Replay mode - loads steps from file, ignores keyboard (Exercise 3)
// Use this class for:
//   - Load mode (-load): silent = false
//   - Silent verification mode (-load -silent): silent = true
class GameFileInput : public Game {
    Steps steps_;
    Results expectedResults_;
    Results actualResults_;
    bool isSilent_ = false;
    bool testPassed_ = true;
    int failedIteration_ = -1;  // -1 = file/init error, else = iteration where failure detected
    bool filesLoaded_ = false;
    mutable size_t waitIterations_ = 0;  // Counter for post-steps iterations (safety limit)
    
public:
    // Constructor: set silent=true for -load -silent mode
    explicit GameFileInput(bool silent = false);
    
protected:
    // Override: get input from file instead of keyboard (skips riddle answers)
    char getNextInput() override;
    
    // Override: get riddle answer from file (playerId == 100)
    char getRiddleInput() override;
    
    // Override: verify results in silent mode
    void onResultEvent(size_t iteration, int playerId, int eventType, int extraData) override;
    
    // Override: skip menu in load mode
    bool shouldShowMenu() const override { return false; }
    
    // Override: disable rendering in silent mode
    bool shouldRender() const override { return !isSilent_; }
    
    // Override: faster sleep for replay
    int getSleepDuration() const override;
    
    // Override: signal when replay is complete (no more steps)
    bool isReplayComplete() const override;
    
    // Override: never wait for input in replay mode (both -load and -load -silent)
    bool shouldWaitForInput() const override { return false; }
    
public:
    // Get test result after run()
    bool didTestPass() const { return testPassed_; }
    int getFailedIteration() const { return failedIteration_; }
    
    // Print test result (for -silent mode)
    // Output: "Test PASSED" or "Test FAILED: <iteration>"
    void printTestResult() const;
    
private:
    // Load steps and results files
    bool loadFiles();
    
    // Verify actual result against expected (simplified - iteration and type only)
    void verifyResult(size_t iteration, int eventType);
};
