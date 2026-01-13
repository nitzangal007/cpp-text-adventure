#pragma once

#include <list>
#include <string>

// Minimal event types for Exercise 3 verification
enum class ResultType {
    StagePassed = 0,   // Screen transition completed
    LifeLost = 1,      // A life was lost (shared lives)
    GameWon = 2,       // Game completed successfully
    GameLost = 3,      // Game over (ran out of lives)
    GameAborted = 4    // User exited via ESC mid-game
};

// Minimal result entry - just iteration and type
struct ResultEntry {
    size_t iteration;  // Game tick when event occurred
    ResultType type;   // Type of event
};

// Records and verifies minimal game events for testing
class Results {
    std::list<ResultEntry> results_;

public:
    // File I/O
    static Results loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Recording (used by GameKeyboardInput)
    void addStagePassed(size_t iteration);
    void addLifeLost(size_t iteration);
    void addGameWon(size_t iteration);
    void addGameLost(size_t iteration);
    void addGameAborted(size_t iteration);
    
    // Verification (used by GameFileInput in silent mode)
    bool hasMoreResults() const { return !results_.empty(); }
    ResultEntry popResult();
    const ResultEntry& peekResult() const;
    
    // Utility
    size_t getResultCount() const { return results_.size(); }
    void clear() { results_.clear(); }
};
