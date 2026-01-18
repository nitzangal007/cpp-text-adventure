#pragma once

#include "Game.h"

// GameFile - Derived class for loading game from files
// Used for: Load mode and Load-Silent mode
class GameFile : public Game
{
private:
    bool silentMode = false;  // If true, no display and fast execution

public:
    // Constructor
    GameFile(bool enableSilentMode = false);
    
    // Start the game from files (no menu in load mode)
    void runFromFiles();

protected:
    // ==========================================
    // Override Virtual Functions for Load Mode
    // ==========================================
    
    // Check if there's input available (from steps file)
    bool hasInput() override;
    
    // Get the next input character (from steps file)
    char getNextInput() override;
    
    // Render - empty in silent mode
    void doRender() override;
    
    // Get sleep duration - faster in load mode, instant in silent mode
    int getSleepDuration() const override;
    
    // Don't show overlays in load mode
    bool shouldShowOverlay() const override;
    
    // Verify results match expected (instead of recording)
    void recordLostLife() override;
    void recordScreenChange(int screenNumber) override;
    void recordRiddle(const std::string& question, const std::string& answer, bool correct) override;
    void recordGameEnded(int finalScore) override;
    void recordBossTaskComplete(int taskNumber) override;
    
    // Check if replay is finished
    bool shouldFinishGame() const override;

    // Override riddle solving to use results from file (skip input)
    bool solveRiddle(class Riddle* r) override;

private:
    // Load files from disk
    bool loadFilesFromDisk();
    
    // Expected results for verification
    Results expectedResults;
    
    // Track verification status
    bool verificationFailed = false;
    std::string failureMessage;
    size_t failureIteration = 0;
    
    // Report verification result
    void reportVerificationResult();
    
    // Helper to check a result against expected
    bool verifyResult(Results::ResultType expectedType, int expectedValue = 0);
};
