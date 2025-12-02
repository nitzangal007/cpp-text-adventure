#pragma once

#include <conio.h>
#include "Player.h"
#include "Screens.h"
#include "Menu.h"

// What should happen when the game loop exits
enum class GameResult { BackToMenu, QuitProgram };

class Game
{
    // ---- Internal helper types ----
    struct Bomb {
        bool  active = false;  // is there a bomb currently placed
        Point pos;                // bomb position on the board
        int   ticksLeft = 0;      // time left until explosion
    };

    // Describes how to move from one screen to the next
    struct ExitInfo {
        Screens::ScreenId from;      // which screen we exit from
        Screens::ScreenId to;        // next screen to move to

        Point doorPos;               // door tile position (the door itself)
        Point waitPos;               // one step AFTER the door (where players wait)

        Point nextStartP1;           // Player 1 start position on the next screen
        Point nextStartP2;           // Player 2 start position on the next screen
    };

    // ---- Core game state ----
    Screens currentScreen;

    Player  player1;
    Player  player2;

    // Initial start positions for the very first screen
    Point   player1Start;
    Point   player2Start;

    // Bomb state
    Bomb bomb;

    // Exit/multi–screen navigation data
    ExitInfo exits[Screens::NUM_SCREENS];

    // Per–player flags for “already reached waitPos and ready to move to next screen”
    bool player1ReadyForNextScreen;
    bool player2ReadyForNextScreen;

public:
    // ---- Ctor / initialization ----
    Game()
        : player1(Player::Id::First, Point(5, 5, 0, 0, '$'), "wdxas", '$'),
        player2(Player::Id::Second, Point(10, 10, 0, 0, '&'), "ilmjk", '&'),
        player1Start(5, 5, 0, 0, '$'),
        player2Start(10, 10, 0, 0, '&'),
        player1ReadyForNextScreen(false),
        player2ReadyForNextScreen(false)
    {
    

        exits[0] = ExitInfo{
            Screens::ScreenId::First,   
            Screens::ScreenId::Second,  
            Point(43, 20, 0, 0, ' '),    
            Point(43, 21, 0, 0, ' '),  
			player1Start,
			player2Start
                
        };

        exits[1] = ExitInfo{
            Screens::ScreenId::Second,
            Screens::ScreenId::Final,
            Point(43, 20, 0, 0, ' '),
            Point(43, 21, 0, 0, ' '),
            Point(25, 5, 0, 0, '$'),
            Point(30, 5, 0, 0, '&')
        };

    }

    // Initialize all game data (players, screens, exits, etc.)
    void initGame();

    // Top–level entry point: show menu, start game, etc.
    void run();

    // Main game loop (single run of the game)
    void runGame();

    // ---- Per–frame update & rendering ----

    // Update all game logic for one tick (players, items, bomb, doors...)
    void updateLogic();

    // Draw current screen, players, status bar, etc.
    void render();

    // Handle movement and collisions for a single player
    void updatePlayerMovement(Player& player);

    // Update rows that depend on switches (raising/lowering walls, etc.)
    void updateSwitchRows();

    // If player stands on a collectible item – pick it up
    void collectItemIfPossible(Player& player);

    // Draw items / inventory state at the bottom of the screen
    void drawStatusBar();

    // ---- Bomb handling ----

    // Place a bomb at the player's position (if allowed)
    void tryPlaceBomb(Player& player);

    // Handle bomb explosion: clear area, hurt players, etc.
    void explodeBomb();

    // Check if a given player is in the explosion radius (helper)
    bool isPlayerInExplosion(const Player& player,
        const Point& center,
        int radiusSquared);

private:
    // ---- Small helper functions for multi–screen logic ----

    // Is this player already standing on waitPos and ready for next screen?
    bool playerIsReadyForNextScreen(const Player& player) const;

    // Check if a point is exactly the “waitPos” (one step after the door)
    bool isExitWaitPosition(const Point& p) const;

    // If both players are ready, move to the next screen
    void tryAdvanceToNextScreen();
};
