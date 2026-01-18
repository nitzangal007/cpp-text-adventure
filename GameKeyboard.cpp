// GameKeyboard - Keyboard input with optional save mode
#include "GameKeyboard.h"
#include "GameConstants.h"
#include <chrono>
#include <filesystem>

GameKeyboard::GameKeyboard(bool enableSaveMode)
    : Game(), saveMode(enableSaveMode)
{
}

void GameKeyboard::run()
{
    if (saveMode) {
        initRecording();
    }
    
    // Call base class run
    Game::run();
    
    // Save files if in save mode
    if (saveMode) {
        saveFilesToDisk();
    }
}

void GameKeyboard::initRecording()
{
    // Clear any previous data
    steps.clear();
    results.clear();
    
    // Set random seed
    long seed = static_cast<long>(std::chrono::system_clock::now().time_since_epoch().count());
    steps.setRandomSeed(seed);
    srand(seed);
    
    // Get list of screen files
    std::vector<std::string> screenFiles;
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        auto filename = entry.path().filename();
        auto filenameStr = filename.string();
        if (filenameStr.find("adv-world") != std::string::npos && 
            filename.extension() == ".screen") {
            screenFiles.push_back(filenameStr);
        }
    }
    // Sort for consistent ordering
    std::sort(screenFiles.begin(), screenFiles.end());
    steps.setScreenFiles(screenFiles);
}

void GameKeyboard::recordStep(char p1Key, char p2Key)
{
    if (saveMode && (p1Key != '\0' || p2Key != '\0')) {
        steps.addStep(iteration, p1Key, p2Key);
    }
}

void GameKeyboard::recordLostLife()
{
    if (saveMode) {
        results.addLostLife(iteration);
    }
}

void GameKeyboard::recordScreenChange(int screenNumber)
{
    if (saveMode) {
        results.addScreenChange(iteration, screenNumber);
    }
}

void GameKeyboard::recordRiddle(const std::string& question, const std::string& answer, bool correct)
{
    if (saveMode) {
        results.addRiddle(iteration, question, answer, correct);
    }
}

void GameKeyboard::recordGameEnded(int finalScore)
{
    if (saveMode) {
        results.addGameEnded(iteration, finalScore);
    }
}

void GameKeyboard::recordBossTaskComplete(int taskNumber)
{
    if (saveMode) {
        results.addBossTaskComplete(iteration, taskNumber);
    }
}

void GameKeyboard::saveFilesToDisk()
{
    steps.saveSteps("adv-world.steps");
    results.saveResults("adv-world.result");
}
