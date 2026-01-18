#pragma once

#include "Game.h"

// GameKeyboard - Derived class for keyboard input with optional recording
// Used for: Normal mode and Save mode
class GameKeyboard : public Game
{
private:
    bool saveMode = false;  // If true, record steps and results to files

public:
    // Constructor
    GameKeyboard(bool enableSaveMode = false);
    
    // Start the game (override to handle save mode)
    void run();

protected:
    // ==========================================
    // Override Virtual Functions for Save Mode
    // ==========================================
    
    // hasInput and getNextInput use base implementation (keyboard)
    
    // Record step - save to steps if in save mode
    void recordStep(char p1Key, char p2Key) override;
    
    // Record events - save to results if in save mode
    void recordLostLife() override;
    void recordScreenChange(int screenNumber) override;
    void recordRiddle(const std::string& question, const std::string& answer, bool correct) override;
    void recordGameEnded(int finalScore) override;
    void recordBossTaskComplete(int taskNumber) override;

private:
    // Helper to save files at end of game
    void saveFilesToDisk();
    
    // Initialize recording for a new game
    void initRecording();
};
