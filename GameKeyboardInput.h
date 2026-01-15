#pragma once

#include "Game.h"
#include "Steps.h"
#include "Results.h"

// Normal game mode with optional recording to files (Exercise 3)
// Use this class for:
//   - Normal mode (no args): saveEnabled_ = false
//   - Save mode (-save): saveEnabled_ = true
class GameKeyboardInput : public Game {
    Steps steps_;
    Results results_;
    bool saveEnabled_ = false;
    bool filesInitialized_ = false;
    
public:
    // Constructor: set saveToFiles=true for -save mode
    explicit GameKeyboardInput(bool saveToFiles = false);
    
    // Destructor: saves files if recording was enabled
    ~GameKeyboardInput() override;
    
protected:
    // Override: record steps when keys are pressed
    void onInputReceived(size_t iteration, int playerId, char key) override;
    
    // Override: record result events
    void onResultEvent(size_t iteration, int playerId, int eventType, int extraData) override;
    
private:
    // Initialize recording (capture screen files, RNG seed)
    void initRecording();
    
    // Save files on exit
    void saveFilesOnExit();
};
