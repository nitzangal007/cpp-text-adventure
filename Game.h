#pragma once

#include <conio.h>
#include <vector>
#include "Player.h"
#include "Screens.h"
#include "Menu.h"

// What should happen when the game loop exits
enum class GameResult { BackToMenu, QuitProgram };

class Game
{
    // ==========================================
    // Internal Structures
    // ==========================================
    struct Bomb {
        bool  active = false;
        Point pos;
        int   ticksLeft = 0;
    };

    struct AutoBomb {
        Point center;
        int   ticksLeft = 0;
    };

    struct ExitInfo {
        Screens::ScreenId from;
        Screens::ScreenId to;
        Point doorPos;
        Point waitPos;
        Point nextStartP1;
        Point nextStartP2;
    };

    // ==========================================
    // Member Variables
    // ==========================================
    Screens currentScreen;
    Player  player1;
    Player  player2;
    Point   player1Start;
    Point   player2Start;

    // Bomb state
    Bomb bomb;
    std::vector<AutoBomb> autoBombs;

    // Navigation
    ExitInfo exits[Screens::NUM_SCREENS];
    bool player1ReadyForNextScreen;
    bool player2ReadyForNextScreen;
    bool gameOver = false;

public:
    Game();

        exits[0] = ExitInfo{
            Screens::ScreenId::First,   
            Screens::ScreenId::Second,  
            Point(43, 20, 0, 0, ' '),    
            Point(43, 21, 0, 0, ' '),  
            Point(54, 9, 0, 0, '$'),
            Point(26, 9, 0, 0, '&')
                
        };

        exits[1] = ExitInfo{
            Screens::ScreenId::Second,
            Screens::ScreenId::Final,
            Point(38, 13),
            Point(38, 13),
            Point(),
            Point()
        };

    }

    // Top–level entry point: show menu, start game, etc.
    // ==========================================
    // Public Interface
    // ==========================================

    // Main entry point: shows menu, handles loops
    void run();

private:
    // ==========================================
    // Core Game Control
    // ==========================================

    // Initialize/Reset game state
    void initGame();
    
    // Main loop for a single game session
    void runGame();
    
    // Reinits current level (e.g. after death)
    void resetCurrentGame();

    // ==========================================
    // Update & Render
    // ==========================================

    // Frame logic: movement, switches, bombs
    void updateLogic();

    // Draw map, players, UI
    void render();
    void drawStatusBar();

    // ==========================================
    // Player Logic
    // ==========================================

    void updatePlayerMovement(Player& player);
    void collectItemIfPossible(Player& player);
    Player& getOtherPlayer(const Player& p);

    // ==========================================
    // Bomb Logic
    // ==========================================

    void tryPlaceBomb(Player& player);
    void explodeBomb();
    
    // Handle auto-bombs (returns true if player died)
    bool handleAutoBombs();

    bool isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared);

    // ==========================================
    // Screen Transition Logic
    // ==========================================

    // Checks readiness and moves to next screen if OK
    void tryAdvanceToNextScreen();

    bool playerIsReadyForNextScreen(const Player& player) const;
    bool isExitWaitPosition(const Point& p) const;
};
