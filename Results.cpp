// Based on lecturer's code, adapted for text adventure game
#include <fstream>
#include <sstream>

#include "Results.h"

Results Results::loadResults(const std::string& filename) {
    Results results;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return results;  // Return empty results if file doesn't exist
    }
    
    // Read number of results
    size_t resultCount;
    file >> resultCount;
    
    // Read each result
    for (size_t i = 0; i < resultCount && !file.eof(); ++i) {
        size_t iteration;
        int typeInt;
        file >> iteration >> typeInt;
        
        ResultEntry entry;
        entry.iteration = iteration;
        entry.type = static_cast<ResultType>(typeInt);
        entry.screenNumber = 0;
        entry.riddleCorrect = false;
        entry.finalScore = 0;
        
        switch (entry.type) {
            case lostLife:
                // No additional data
                break;
                
            case screenChange:
                file >> entry.screenNumber;
                break;
                
            case riddle: {
                // Read: "question" "answer" correct(0/1)
                std::string line;
                std::getline(file, line);  // Get rest of line
                
                // Parse quoted strings
                size_t pos1 = line.find('"');
                size_t pos2 = line.find('"', pos1 + 1);
                size_t pos3 = line.find('"', pos2 + 1);
                size_t pos4 = line.find('"', pos3 + 1);
                
                if (pos1 != std::string::npos && pos2 != std::string::npos) {
                    entry.riddleQuestion = line.substr(pos1 + 1, pos2 - pos1 - 1);
                }
                if (pos3 != std::string::npos && pos4 != std::string::npos) {
                    entry.riddleAnswer = line.substr(pos3 + 1, pos4 - pos3 - 1);
                }
                
                // Find the boolean at the end
                size_t lastSpace = line.find_last_of(' ');
                if (lastSpace != std::string::npos) {
                    entry.riddleCorrect = (line[lastSpace + 1] == '1');
                }
                break;
            }
                
            case gameEnded:
                file >> entry.finalScore;
                break;
                
            case bossTaskComplete:
                file >> entry.bossTaskNumber;
                break;
                
            default:
                break;
        }
        
        results.results.push_back(entry);
    }
    
    file.close();
    return results;
}

void Results::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    
    // Write number of results
    file << results.size();
    
    // Write each result
    for (const auto& result : results) {
        file << '\n' << result.iteration << ' ' << static_cast<int>(result.type);
        
        switch (result.type) {
            case lostLife:
                // No additional data
                break;
                
            case screenChange:
                file << ' ' << result.screenNumber;
                break;
                
            case riddle:
                file << " \"" << result.riddleQuestion << "\" \"" 
                     << result.riddleAnswer << "\" " << (result.riddleCorrect ? 1 : 0);
                break;
                
            case gameEnded:
                file << ' ' << result.finalScore;
                break;
                
            case bossTaskComplete:
                file << ' ' << result.bossTaskNumber;
                break;
                
            default:
                break;
        }
    }
    
    file.close();
}

void Results::addLostLife(size_t iteration) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.type = lostLife;
    entry.screenNumber = 0;
    entry.riddleCorrect = false;
    entry.finalScore = 0;
    results.push_back(entry);
}

void Results::addScreenChange(size_t iteration, int screenNumber) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.type = screenChange;
    entry.screenNumber = screenNumber;
    entry.riddleCorrect = false;
    entry.finalScore = 0;
    results.push_back(entry);
}

void Results::addRiddle(size_t iteration, const std::string& question,
                        const std::string& answer, bool correct) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.type = riddle;
    entry.screenNumber = 0;
    entry.riddleQuestion = question;
    entry.riddleAnswer = answer;
    entry.riddleCorrect = correct;
    entry.finalScore = 0;
    results.push_back(entry);
}

void Results::addGameEnded(size_t iteration, int score) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.type = gameEnded;
    entry.screenNumber = 0;
    entry.riddleCorrect = false;
    entry.finalScore = score;
    entry.bossTaskNumber = 0;
    results.push_back(entry);
}

void Results::addBossTaskComplete(size_t iteration, int taskNumber) {
    ResultEntry entry;
    entry.iteration = iteration;
    entry.type = bossTaskComplete;
    entry.screenNumber = 0;
    entry.riddleCorrect = false;
    entry.finalScore = 0;
    entry.bossTaskNumber = taskNumber;
    results.push_back(entry);
}

Results::ResultEntry Results::popResult() {
    if (results.empty()) {
        ResultEntry empty;
        empty.iteration = 0;
        empty.type = noResult;
        empty.screenNumber = 0;
        empty.riddleCorrect = false;
        empty.finalScore = 0;
        return empty;
    }
    ResultEntry result = results.front();
    results.pop_front();
    return result;
}

Results::ResultEntry Results::peekResult() const {
    if (results.empty()) {
        ResultEntry empty;
        empty.iteration = 0;
        empty.type = noResult;
        empty.screenNumber = 0;
        empty.riddleCorrect = false;
        empty.finalScore = 0;
        return empty;
    }
    return results.front();
}

bool Results::isFinishedBy(size_t iteration) const {
    return results.empty() || results.back().iteration <= iteration;
}

void Results::clear() {
    results.clear();
}
