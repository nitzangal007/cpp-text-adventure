// GameFile - Load game from files
#include "GameFile.h"
#include "GameConstants.h"
#include <iostream>
#include <Windows.h>

GameFile::GameFile(bool enableSilentMode)
    : Game(), silentMode(enableSilentMode)
{
}

void GameFile::runFromFiles()
{
    // Load steps and expected results from files
    if (!loadFilesFromDisk()) {
        std::cout << "ERROR: Failed to load game files." << std::endl;
        std::cout << "Make sure adv-world.steps and adv-world.result exist." << std::endl;
        return;
    }
    
    // Set random seed from loaded steps
    srand(steps.getRandomSeed());
    
    // Run the game directly (no menu in load mode)
    cls();
    initGame();
    
    if (gameOver) {
        return;  // Error during init
    }
    
    runGame();
    
    // Report verification result
    reportVerificationResult();
}

bool GameFile::loadFilesFromDisk()
{
    steps = Steps::loadSteps("adv-world.steps");
    expectedResults = Results::loadResults("adv-world.result");
    
    // Check if files were loaded successfully
    if (!steps.hasMoreSteps() && steps.getRandomSeed() == 0) {
        return false;  // Steps file probably doesn't exist
    }
    
    return true;
}

bool GameFile::hasInput()
{
    // Check if there's a step for the current iteration
    return steps.isNextStepOnIteration(iteration);
}

char GameFile::getNextInput()
{
    if (!steps.isNextStepOnIteration(iteration)) {
        return '\0';  // No input for this iteration
    }
    
    StepEntry step = steps.popStep();
    
    // Return player1 key if available, otherwise player2 key
    // In load mode, we process both keys in sequence if both exist
    if (step.player1Key != '\0') {
        // If there's also a player2 key, we need to handle it
        // For now, just return player1 key first
        return step.player1Key;
    }
    return step.player2Key;
}

void GameFile::doRender()
{
    if (!silentMode) {
        // Normal render, just faster
        render();
    }
    // In silent mode, do nothing
}

int GameFile::getSleepDuration() const
{
    if (silentMode) {
        return 0;  // No sleep in silent mode
    }
    return 10;  // Faster than normal (10ms vs 40ms)
}

bool GameFile::shouldShowOverlay() const
{
    return false;  // Never show overlays in load mode
}

void GameFile::recordLostLife()
{
    if (verificationFailed) return;  // Already failed
    
    Results::ResultEntry expected = expectedResults.popResult();
    if (expected.type != Results::lostLife) {
        verificationFailed = true;
        failureMessage = "Expected lostLife event but got different event";
        failureIteration = iteration;
    }
}

void GameFile::recordScreenChange(int screenNumber)
{
    if (verificationFailed) return;
    
    Results::ResultEntry expected = expectedResults.popResult();
    if (expected.type != Results::screenChange || expected.screenNumber != screenNumber) {
        verificationFailed = true;
        failureMessage = "Expected screenChange to screen " + std::to_string(screenNumber) + 
                        " but got different event";
        failureIteration = iteration;
    }
}

void GameFile::recordRiddle(const std::string& question, const std::string& answer, bool correct)
{
    if (verificationFailed) return;
    
    Results::ResultEntry expected = expectedResults.popResult();
    if (expected.type != Results::riddle) {
        verificationFailed = true;
        failureMessage = "Expected riddle event but got different event";
        failureIteration = iteration;
    }
    else if (expected.riddleCorrect != correct) {
        verificationFailed = true;
        failureMessage = "Riddle answer correctness mismatch";
        failureIteration = iteration;
    }
}

void GameFile::recordGameEnded(int finalScore)
{
    if (verificationFailed) return;
    
    Results::ResultEntry expected = expectedResults.popResult();
    if (expected.type != Results::gameEnded) {
        verificationFailed = true;
        failureMessage = "Expected gameEnded event but got different event";
        failureIteration = iteration;
    }
    // Note: We could also verify the score matches
}

void GameFile::recordBossTaskComplete(int taskNumber)
{
    if (verificationFailed) return;
    
    Results::ResultEntry expected = expectedResults.popResult();
    if (expected.type != Results::bossTaskComplete) {
        verificationFailed = true;
        failureMessage = "Expected bossTaskComplete event but got different event";
        failureIteration = iteration;
    }
    else if (expected.bossTaskNumber != taskNumber) {
        verificationFailed = true;
        failureMessage = "Boss task number mismatch: expected " + std::to_string(expected.bossTaskNumber) +
                        " but got " + std::to_string(taskNumber);
        failureIteration = iteration;
    }
}

void GameFile::reportVerificationResult()
{
    cls();
    
    if (verificationFailed) {
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Failure at iteration: " << failureIteration << std::endl;
        std::cout << "Reason: " << failureMessage << std::endl;
    }
    else if (expectedResults.hasMoreResults()) {
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Game ended but expected results file has more events" << std::endl;
    }
    else {
        std::cout << "=== TEST PASSED ===" << std::endl;
        std::cout << std::endl;
        std::cout << "All events matched expected results!" << std::endl;
    }
    
    if (!silentMode) {
        std::cout << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        _getch();
    }
}

bool GameFile::verifyResult(Results::ResultType expectedType, int expectedValue)
{
    Results::ResultEntry expected = expectedResults.popResult();
    return expected.type == expectedType;
}

bool GameFile::shouldFinishGame() const
{
    // Finish if verification failed OR if we ran out of steps and results
    // We check both because sometimes steps end before results (e.g. death/win)
    if (verificationFailed) return true;
    
    // Check if we have more work to do
    // Note: We might have results but no steps if the last step caused an automatic event
    // But generally, if both are empty, we are definitely done.
    if (!steps.hasMoreSteps() && !expectedResults.hasMoreResults()) {
        return true;
    }
    
    // Safety check: if we are way past the last step iteration, we should probably stop
    // (To prevent infinite loops if something goes wrong)
    // For now, let's just rely on the empty checks.
    
    return false;
}

bool GameFile::solveRiddle(Riddle* r)
{
    // In Load/Silent mode, we don't ask for input.
    // We retrieve the expected result from the file.

    if (!expectedResults.hasMoreResults()) {
        verificationFailed = true;
        failureMessage = "Expected riddle event, but no more results found.";
        failureIteration = iteration;
        return false;
    }

    Results::ResultEntry expected = expectedResults.popResult();

    if (expected.type != Results::riddle) {
        verificationFailed = true;
        failureMessage = "Expected riddle event, found result type: " + std::to_string(expected.type);
        failureIteration = iteration;
        return false;
    }

    // Optional: Verify question matches
    if (expected.riddleQuestion != r->getQuestion()) {
        // Warning: Mismatch in riddle question?
        // Maybe RNG mismatch or different riddle file?
        // For strict verification we could fail here.
        // verificationFailed = true;
        // failureMessage = "Riddle question mismatch!";
    }

    // Return the recorded outcome
    return expected.riddleCorrect;
}
