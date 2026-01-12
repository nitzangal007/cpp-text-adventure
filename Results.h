#pragma once

#include <list>
#include <string>

// Types of result events to track
enum class ResultType {
    ScreenTransition = 0,  // Player moved to another screen
    LifeLost = 1,          // Player lost a life
    RiddleEncounter = 2,   // Player encountered a riddle
    GameFinished = 3       // Game completed
};

// Represents a single result event
struct ResultEntry {
    size_t iteration;      // Game cycle when event occurred
    int playerId;          // 1 or 2 (0 for game-wide events)
    ResultType type;       // Type of event
    
    // Extra data depending on type:
    int screenId = 0;      // For ScreenTransition: destination screen
    int riddleId = 0;      // For RiddleEncounter: which riddle
    bool riddleCorrect = false;  // For RiddleEncounter: was answer correct
    int finalScore = 0;    // For GameFinished: final score
};

// Records and verifies game result events for testing
class Results {
    std::list<ResultEntry> results_;

public:
    // File I/O
    static Results loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Recording (used by GameKeyboardInput)
    void addScreenTransition(size_t iteration, int playerId, int toScreenId);
    void addLifeLost(size_t iteration, int playerId);
    void addRiddleEncounter(size_t iteration, int playerId, int riddleId, bool correct);
    void addGameFinished(size_t iteration, int finalScore);
    
    // Verification (used by GameFileInput in silent mode)
    bool hasMoreResults() const { return !results_.empty(); }
    ResultEntry popResult();
    const ResultEntry& peekResult() const;
    
    // Utility
    size_t getResultCount() const { return results_.size(); }
    void clear() { results_.clear(); }
};
