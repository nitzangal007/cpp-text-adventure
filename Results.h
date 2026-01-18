#pragma once

#include <list>
#include <string>

// Based on lecturer's code, adapted for text adventure game requirements
// Supports: lostLife, screenChange, riddle solved, gameEnded

class Results {
public:
    enum ResultType {
        lostLife = 0,       // Player lost a life
        screenChange = 1,   // Player moved to another screen
        riddle = 2,         // Riddle encountered
        gameEnded = 3,      // Game finished
        bossTaskComplete = 4, // Boss task completed (1, 2, or 3)
        noResult = -1       // No result (for empty pops)
    };

    struct ResultEntry {
        size_t iteration;
        ResultType type;
        int screenNumber;          // For screenChange: which screen
        std::string riddleQuestion; // For riddle: the question
        std::string riddleAnswer;   // For riddle: the answer given
        bool riddleCorrect;         // For riddle: was answer correct
        int finalScore;             // For gameEnded: final score
        int bossTaskNumber;         // For bossTaskComplete: which task (1, 2, or 3)
    };

private:
    std::list<ResultEntry> results;

public:
    // Load results from file
    static Results loadResults(const std::string& filename);
    
    // Save results to file
    void saveResults(const std::string& filename) const;
    
    // Add a "lost life" event
    void addLostLife(size_t iteration);
    
    // Add a "screen change" event
    void addScreenChange(size_t iteration, int screenNumber);
    
    // Add a "riddle" event
    void addRiddle(size_t iteration, const std::string& question, 
                   const std::string& answer, bool correct);
    
    // Add a "game ended" event
    void addGameEnded(size_t iteration, int score);
    
    // Add a "boss task complete" event
    void addBossTaskComplete(size_t iteration, int taskNumber);
    
    // Get and remove the next result
    ResultEntry popResult();
    
    // Peek at the next result without removing
    ResultEntry peekResult() const;
    
    // Check if results are exhausted by this iteration
    bool isFinishedBy(size_t iteration) const;
    
    // Check if there are more results
    bool hasMoreResults() const { return !results.empty(); }
    
    // Clear all results
    void clear();
};
