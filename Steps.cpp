#include "Steps.h"
#include <fstream>
#include <sstream>

// ==========================================
// File I/O
// ==========================================

Steps Steps::loadFromFile(const std::string& filename) {
    Steps steps;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return steps;  // Return empty steps if file doesn't exist
    }
    
    // Line 1: Random seed
    file >> steps.randomSeed_;
    
    // Line 2: Screen files (comma-separated)
    std::string screenFilesLine;
    std::getline(file, screenFilesLine);  // Consume newline after seed
    std::getline(file, screenFilesLine);  // Get actual screen files line
    
    // Parse comma-separated screen files
    std::stringstream ss(screenFilesLine);
    std::string screenFile;
    while (std::getline(ss, screenFile, ',')) {
        if (!screenFile.empty()) {
            steps.screenFiles_.push_back(screenFile);
        }
    }
    
    // Remaining lines: iteration playerId key
    size_t iteration;
    int playerId;
    char key;
    while (file >> iteration >> playerId >> key) {
        steps.steps_.push_back({iteration, playerId, key});
    }
    
    file.close();
    return steps;
}

bool Steps::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    // Line 1: Random seed
    file << randomSeed_ << '\n';
    
    // Line 2: Screen files (comma-separated)
    for (size_t i = 0; i < screenFiles_.size(); ++i) {
        if (i > 0) file << ',';
        file << screenFiles_[i];
    }
    file << '\n';
    
    // Remaining lines: iteration playerId key
    for (const auto& step : steps_) {
        file << step.iteration << ' ' << step.playerId << ' ' << step.key << '\n';
    }
    
    file.close();
    return true;
}

// ==========================================
// Recording
// ==========================================

void Steps::addStep(size_t iteration, int playerId, char key) {
    steps_.push_back({iteration, playerId, key});
}

// ==========================================
// Playback
// ==========================================

bool Steps::isNextStepOnIteration(size_t iteration) const {
    return !steps_.empty() && steps_.front().iteration == iteration;
}

StepEntry Steps::popStep() {
    StepEntry step = steps_.front();
    steps_.pop_front();
    return step;
}

const StepEntry& Steps::peekStep() const {
    return steps_.front();
}

// ==========================================
// Utility
// ==========================================

void Steps::clear() {
    randomSeed_ = 0;
    screenFiles_.clear();
    steps_.clear();
}
