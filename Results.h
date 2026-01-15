#pragma once

#include <string>
#include <deque>
#include <cstdint>

// Result types for Exercise 3 recording/verification
enum class ResultType {
    StagePassed = 0,   // Screen transition
    LifeLost = 1,      // Player died
    GameWon = 2,       // Game completed successfully
    GameLost = 3,      // Game over (no lives)
    GameAborted = 4    // User exited via ESC->H
};

// Single result entry: iteration + type
struct ResultEntry {
    size_t iteration;
    ResultType type;
};

class Results {
    std::deque<ResultEntry> results_;

public:
    // File I/O (auto-detects old vs new format)
    static Results loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Recording
    void addStagePassed(size_t iteration);
    void addLifeLost(size_t iteration);
    void addGameWon(size_t iteration);
    void addGameLost(size_t iteration);
    void addGameAborted(size_t iteration);
    
    // Verification
    bool hasMoreResults() const { return !results_.empty(); }
    ResultEntry popResult();
    const ResultEntry& peekResult() const;
    size_t getResultCount() const { return results_.size(); }
};
