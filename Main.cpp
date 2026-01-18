// Main entry point - handles command line arguments for save/load modes
#include "Game.h"
#include "GameKeyboard.h"
#include "GameFile.h"
#include <string>

int main(int argc, char* argv[]) {
    hideCursor();
    
    // Parse command line arguments
    bool loadMode = false;
    bool saveMode = false;
    bool silentMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-load") {
            loadMode = true;
        }
        else if (arg == "-save") {
            saveMode = true;
        }
        else if (arg == "-silent") {
            silentMode = true;
        }
    }
    
    // Run appropriate game mode
    if (loadMode) {
        // Load mode: play from files (no menu)
        GameFile game(silentMode);
        game.runFromFiles();
    }
    else if (saveMode) {
        // Save mode: normal game with recording
        GameKeyboard game(true);  // true = enable save mode
        game.run();
    }
    else {
        // Normal mode: regular game (no save/load)
        Game game;
        game.run();
    }
    
    return 0;
}