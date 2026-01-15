#include "GameKeyboardInput.h"
#include <iostream>

GameKeyboardInput::GameKeyboardInput(bool saveToFiles)
    : Game()
    , saveEnabled_(saveToFiles)
    , filesInitialized_(false)
{
}

GameKeyboardInput::~GameKeyboardInput()
{
    if (saveEnabled_ && filesInitialized_) {
        saveFilesOnExit();
    }
}

void GameKeyboardInput::initRecording()
{
    if (!saveEnabled_ || filesInitialized_) {
        return;
    }
    
    // Capture screen file names
    steps_.setScreenFiles(currentScreen.getScreenFileNames());
    
    // Capture Room3Boss RNG seed for deterministic replay
    steps_.setRandomSeed(room3Boss.getRngSeed());
    
    filesInitialized_ = true;
}

void GameKeyboardInput::onInputReceived(size_t iteration, int playerId, char key)
{
    if (!saveEnabled_) {
        return;
    }
    
    // Initialize on first input (ensures screen files are already loaded)
    if (!filesInitialized_) {
        initRecording();
    }
    
    steps_.addStep(iteration, playerId, key);
}

void GameKeyboardInput::onResultEvent(size_t iteration, int playerId, int eventType, int extraData)
{
    if (!saveEnabled_) {
        return;
    }
    
    // Initialize if not already done
    if (!filesInitialized_) {
        initRecording();
    }
    
    // New minimal event types:
    //   0 = StagePassed (screen transition)
    //   1 = LifeLost
    //   2 = Riddle (now ignored - not tracked)
    //   3 = GameFinished (extraData >= 0 = won, < 0 = lost)
    
    switch (eventType) {
    case 0:  // StagePassed
        results_.addStagePassed(iteration);
        break;
    case 1:  // LifeLost
        results_.addLifeLost(iteration);
        break;
    case 2:  // Riddle - no longer tracked
        // Do nothing
        break;
    case 3:  // GameFinished
        if (extraData >= 0) {
            results_.addGameWon(iteration);
        } else {
            results_.addGameLost(iteration);
        }
        break;
    case 4:  // GameAborted (user exit via ESC)
        results_.addGameAborted(iteration);
        break;
    }
}

void GameKeyboardInput::saveFilesOnExit()
{
    steps_.saveToFile("adv-world.steps");
    results_.saveToFile("adv-world.result");
    
    std::cout << "\nGame recorded. Files saved:\n";
    std::cout << "  - adv-world.steps (" << steps_.getStepCount() << " steps)\n";
    std::cout << "  - adv-world.result (" << results_.getResultCount() << " results)\n";
}
