#pragma once

#include <list>
#include <string>
#include <vector>

// Based on lecturer's code, adapted for two-player game with single file for all screens
// Each step can contain input from both players on the same iteration

struct StepEntry {
    size_t iteration;
    char player1Key;  // '\0' if no input from player 1
    char player2Key;  // '\0' if no input from player 2
};

class Steps {
    long randomSeed = 0;
    std::vector<std::string> screenFiles;  // List of screen files used in this game
    std::list<StepEntry> steps;

public:
    // Load steps from file
    static Steps loadSteps(const std::string& filename);
    
    // Save steps to file
    void saveSteps(const std::string& filename) const;
    
    // Random seed management
    long getRandomSeed() const { return randomSeed; }
    void setRandomSeed(long seed) { randomSeed = seed; }
    
    // Screen files management
    const std::vector<std::string>& getScreenFiles() const { return screenFiles; }
    void setScreenFiles(const std::vector<std::string>& files) { screenFiles = files; }
    
    // Add a step (can have input from one or both players)
    void addStep(size_t iteration, char p1Key, char p2Key);
    
    // Check if there's a step on this iteration
    bool isNextStepOnIteration(size_t iteration) const;
    
    // Get and remove the next step
    StepEntry popStep();
    
    // Check if there are more steps
    bool hasMoreSteps() const { return !steps.empty(); }
    
    // Clear all steps (for starting a new game)
    void clear();
};
