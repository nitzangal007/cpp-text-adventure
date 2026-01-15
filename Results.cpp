#include "Results.h"
#include <fstream>
#include <sstream>

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
    std::string firstLine;
    std::getline(file, firstLine);
    size_t count = 0;
    std::istringstream(firstLine) >> count;
    
    // Detect format by checking first data line
    // NEW format: iteration type (2 fields)
    // OLD format: iteration playerId type [extra...] (3+ fields)
    bool isNewFormat = true;
    
    if (count > 0) {
        std::streampos dataStart = file.tellg();
        std::string sampleLine;
        if (std::getline(file, sampleLine)) {
            // Count fields in sample line
            std::istringstream iss(sampleLine);
            int fieldCount = 0;
            std::string field;
            while (iss >> field) fieldCount++;
            
            isNewFormat = (fieldCount == 2);
        }
        // Seek back to read data properly
        file.seekg(dataStart);
    }
    
    // Parse based on detected format
    for (size_t i = 0; i < count && !file.eof(); ++i) {
        std::string line;
        if (!std::getline(file, line)) break;
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        size_t iteration;
        int typeInt;
        
        if (isNewFormat) {
            // NEW format: iteration type
            iss >> iteration >> typeInt;
        } else {
            // OLD format: iteration playerId type [extra...]
            int playerId;
            iss >> iteration >> playerId >> typeInt;
            // Ignore extra fields
        }
        
        ResultEntry entry;
        entry.iteration = iteration;
        entry.type = static_cast<ResultType>(typeInt);
        
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
    
    // Each line: iteration type (NEW format - 2 fields only)
    for (const auto& entry : results_) {
        file << entry.iteration << ' ' 
             << static_cast<int>(entry.type) << '\n';
    }
    
    file.close();
    return true;
}

// ==========================================
// Recording
// ==========================================

void Results::addStagePassed(size_t iteration) {
    results_.push_back({iteration, ResultType::StagePassed});
}

void Results::addLifeLost(size_t iteration) {
    results_.push_back({iteration, ResultType::LifeLost});
}

void Results::addGameWon(size_t iteration) {
    results_.push_back({iteration, ResultType::GameWon});
}

void Results::addGameLost(size_t iteration) {
    results_.push_back({iteration, ResultType::GameLost});
}

void Results::addGameAborted(size_t iteration) {
    results_.push_back({iteration, ResultType::GameAborted});
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
