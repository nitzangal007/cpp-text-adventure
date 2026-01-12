#pragma once

#include <list>
#include <string>
#include <vector>

// Represents a single step (key press) in the game
struct StepEntry {
    size_t iteration;   // Game cycle when the key was pressed
    int playerId;       // 1 or 2
    char key;           // The key that was pressed
};

// Records and replays game steps for deterministic testing
class Steps {
    unsigned long randomSeed_ = 0;              // For Room3Boss RNG
    std::vector<std::string> screenFiles_;      // Screen files used in this game
    std::list<StepEntry> steps_;                // Recorded steps

public:
    // File I/O
    static Steps loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Seed management (for Room3Boss RNG)
    void setRandomSeed(unsigned long seed) { randomSeed_ = seed; }
    unsigned long getRandomSeed() const { return randomSeed_; }
    
    // Screen file management
    void setScreenFiles(const std::vector<std::string>& files) { screenFiles_ = files; }
    const std::vector<std::string>& getScreenFiles() const { return screenFiles_; }
    
    // Recording (used by GameKeyboardInput)
    void addStep(size_t iteration, int playerId, char key);
    
    // Playback (used by GameFileInput)
    bool hasMoreSteps() const { return !steps_.empty(); }
    bool isNextStepOnIteration(size_t iteration) const;
    StepEntry popStep();
    const StepEntry& peekStep() const;
    
    // Utility
    size_t getStepCount() const { return steps_.size(); }
    void clear();
};
