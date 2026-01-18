// Based on lecturer's code, adapted for two-player game
#include <fstream>
#include <sstream>

#include "Steps.h"

Steps Steps::loadSteps(const std::string& filename) {
    Steps steps;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return steps;  // Return empty steps if file doesn't exist
    }
    
    // Read random seed
    file >> steps.randomSeed;
    
    // Read screen files list (comma-separated on one line)
    std::string screenFilesLine;
    std::getline(file, screenFilesLine);  // consume newline after seed
    std::getline(file, screenFilesLine);
    
    // Parse comma-separated screen files
    std::stringstream ss(screenFilesLine);
    std::string screenFile;
    while (std::getline(ss, screenFile, ',')) {
        if (!screenFile.empty()) {
            steps.screenFiles.push_back(screenFile);
        }
    }
    
    // Read number of steps
    size_t stepCount;
    file >> stepCount;
    
    // Read each step
    for (size_t i = 0; i < stepCount && !file.eof(); ++i) {
        size_t iteration;
        file >> iteration;
        
        // Read the rest of the line for keys
        std::string keysLine;
        std::getline(file, keysLine);  // consume space and get keys
        
        char p1Key = '\0';
        char p2Key = '\0';
        
        // Parse keys from the line (format: " key1 key2" or " key1" or " key2")
        std::stringstream keyStream(keysLine);
        std::string key1, key2;
        
        if (keyStream >> key1) {
            p1Key = key1[0];
            if (keyStream >> key2) {
                p2Key = key2[0];
            }
        }
        
        steps.steps.push_back({ iteration, p1Key, p2Key });
    }
    
    file.close();
    return steps;
}

void Steps::saveSteps(const std::string& filename) const {
    std::ofstream file(filename);
    
    // Write random seed
    file << randomSeed << '\n';
    
    // Write screen files (comma-separated)
    for (size_t i = 0; i < screenFiles.size(); ++i) {
        if (i > 0) file << ',';
        file << screenFiles[i];
    }
    file << '\n';
    
    // Write number of steps
    file << steps.size();
    
    // Write each step
    for (const auto& step : steps) {
        file << '\n' << step.iteration;
        if (step.player1Key != '\0') {
            file << ' ' << step.player1Key;
        }
        if (step.player2Key != '\0') {
            file << ' ' << step.player2Key;
        }
    }
    
    file.close();
}

void Steps::addStep(size_t iteration, char p1Key, char p2Key) {
    // If there's already a step on this iteration, merge the keys
    if (!steps.empty() && steps.back().iteration == iteration) {
        if (p1Key != '\0') {
            steps.back().player1Key = p1Key;
        }
        if (p2Key != '\0') {
            steps.back().player2Key = p2Key;
        }
    }
    else {
        steps.push_back({ iteration, p1Key, p2Key });
    }
}

bool Steps::isNextStepOnIteration(size_t iteration) const {
    return !steps.empty() && steps.front().iteration == iteration;
}

StepEntry Steps::popStep() {
    if (steps.empty()) {
        return { 0, '\0', '\0' };
    }
    StepEntry step = steps.front();
    steps.pop_front();
    return step;
}

void Steps::clear() {
    steps.clear();
    screenFiles.clear();
    randomSeed = 0;
}
