#include "Results.h"
#include <fstream>

// ==========================================
// File I/O
// ==========================================

Results Results::loadFromFile(const std::string& filename) {
    Results results;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return results;  // Return empty results if file doesn't exist
    }
    
    // Line 1: Number of results
    size_t count;
    file >> count;
    
    // Each line: iteration playerId type [extra_data...]
    for (size_t i = 0; i < count && !file.eof(); ++i) {
        size_t iteration;
        int playerId;
        int typeInt;
        
        file >> iteration >> playerId >> typeInt;
        
        ResultEntry entry;
        entry.iteration = iteration;
        entry.playerId = playerId;
        entry.type = static_cast<ResultType>(typeInt);
        
        // Read extra data based on type
        switch (entry.type) {
            case ResultType::ScreenTransition:
                file >> entry.screenId;
                break;
            case ResultType::LifeLost:
                // No extra data
                break;
            case ResultType::RiddleEncounter:
                file >> entry.riddleId;
                int correct;
                file >> correct;
                entry.riddleCorrect = (correct != 0);
                break;
            case ResultType::GameFinished:
                file >> entry.finalScore;
                break;
        }
        
        results.results_.push_back(entry);
    }
    
    file.close();
    return results;
}

bool Results::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    // Line 1: Number of results
    file << results_.size() << '\n';
    
    // Each line: iteration playerId type [extra_data...]
    for (const auto& entry : results_) {
        file << entry.iteration << ' ' 
             << entry.playerId << ' ' 
             << static_cast<int>(entry.type);
        
        // Write extra data based on type
        switch (entry.type) {
            case ResultType::ScreenTransition:
                file << ' ' << entry.screenId;
                break;
            case ResultType::LifeLost:
                // No extra data
                break;
            case ResultType::RiddleEncounter:
                file << ' ' << entry.riddleId << ' ' << (entry.riddleCorrect ? 1 : 0);
                break;
            case ResultType::GameFinished:
                file << ' ' << entry.finalScore;
                break;
        }
        
        file << '\n';
    }
    
    file.close();
    return true;
}

// ==========================================
// Recording
// ==========================================

void Results::addScreenTransition(size_t iteration, int playerId, int toScreenId) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.playerId = playerId;
    entry.type = ResultType::ScreenTransition;
    entry.screenId = toScreenId;
    results_.push_back(entry);
}

void Results::addLifeLost(size_t iteration, int playerId) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.playerId = playerId;
    entry.type = ResultType::LifeLost;
    results_.push_back(entry);
}

void Results::addRiddleEncounter(size_t iteration, int playerId, int riddleId, bool correct) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.playerId = playerId;
    entry.type = ResultType::RiddleEncounter;
    entry.riddleId = riddleId;
    entry.riddleCorrect = correct;
    results_.push_back(entry);
}

void Results::addGameFinished(size_t iteration, int finalScore) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.playerId = 0;  // Game-wide event
    entry.type = ResultType::GameFinished;
    entry.finalScore = finalScore;
    results_.push_back(entry);
}

// ==========================================
// Verification
// ==========================================

ResultEntry Results::popResult() {
    ResultEntry entry = results_.front();
    results_.pop_front();
    return entry;
}

const ResultEntry& Results::peekResult() const {
    return results_.front();
}
