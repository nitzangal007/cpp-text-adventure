
#include "Game.h"
#include "GameKeyboardInput.h"
#include "GameFileInput.h"
#include <memory>
#include <cstring>

// Helper: check if a command-line argument exists
bool hasArg(int argc, char** argv, const char* arg) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], arg) == 0) {
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv) {
    // Parse command-line arguments
    bool isLoad = hasArg(argc, argv, "-load");
    bool isSave = hasArg(argc, argv, "-save");
    bool isSilent = hasArg(argc, argv, "-silent");
    
    // Only manipulate console if not in silent mode
    if (!isSilent) {
        hideCursor();
    }
    
    // Create the appropriate Game subclass
    std::unique_ptr<Game> game;
    
    if (isLoad) {
        // Replay mode: load steps from file
        // - GameFileInput(false) = visual replay
        // - GameFileInput(true) = silent verification
        game = std::make_unique<GameFileInput>(isSilent);
    } else {
        // Normal mode: keyboard input
        // - GameKeyboardInput(false) = normal play
        // - GameKeyboardInput(true) = normal play + save to files
        game = std::make_unique<GameKeyboardInput>(isSave);
    }
    
    // Run the game
    game->run();
    
    // Print test result in silent mode
    if (isLoad && isSilent) {
        static_cast<GameFileInput*>(game.get())->printTestResult();
    }
    
    return 0;
}