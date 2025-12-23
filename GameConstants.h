#pragma once

// ==========================================
// Tile Characters (Single Source of Truth)
// ==========================================
namespace Tiles {
    constexpr char EMPTY_SPACE = ' ';
    constexpr char WALL = 'W';
    constexpr char KEY = 'K';
    constexpr char BOMB = '@';
    constexpr char TORCH = '!';
    constexpr char OBSTACLE = '*';
    constexpr char SWITCH_ON = '/';
    constexpr char SWITCH_OFF = '\\';
    constexpr char RIDDLE = '?';
    constexpr char AUTO_BOMB = 'B';
    constexpr char HINT = 'H';
    constexpr char UNBREAKABLE_WALL = 'X';
    constexpr char LEGEND_ANCHOR = 'L';
    constexpr char SPRING = '#';
}

// ==========================================
// Player Symbols
// ==========================================
namespace Players {
    constexpr char PLAYER1_SYMBOL = '$';
    constexpr char PLAYER2_SYMBOL = '&';
}

// ==========================================
// Input Keys
// ==========================================
namespace Keys {
    constexpr char ESC = 27;
    constexpr char PLAYER1_ACTION = 'E';  // Bomb/drop for player 1
    constexpr char PLAYER2_ACTION = 'O';  // Bomb/drop for player 2
    constexpr char RESTART = 'R';
    constexpr char MENU = 'H';
}

// ==========================================
// Timing Constants (in game ticks or ms)
// ==========================================
namespace Timing {
    constexpr int GAME_TICK_MS = 100;           // Main loop sleep time
    constexpr int BOMB_FUSE_TICKS = 5;          // Ticks until player bomb explodes
    constexpr int AUTO_BOMB_DELAY_TICKS = 21;   // Ticks for auto-bomb timer
    constexpr int BOMB_BLINK_THRESHOLD = 5;     // Start blinking at this many ticks
    constexpr int AUTO_BOMB_BLINK_THRESHOLD = 18;
}

// ==========================================
// Explosion & Torch Radius
// ==========================================
namespace Radius {
    constexpr int BOMB_EXPLOSION = 3;
    constexpr int TORCH_LIGHT = 5;
}

// ==========================================
// Score System
// ==========================================
namespace Score {
    // Time thresholds (in seconds)
    constexpr int TIER1_SECONDS = 60;    // Under 1 minute
    constexpr int TIER2_SECONDS = 120;   // 1-2 minutes
    constexpr int TIER3_SECONDS = 180;   // 2-3 minutes
    constexpr int TIER4_SECONDS = 300;   // 3-5 minutes
    
    // Points per tier
    constexpr int TIER1_POINTS = 2000;
    constexpr int TIER2_POINTS = 1500;
    constexpr int TIER3_POINTS = 1000;
    constexpr int TIER4_POINTS = 500;
    constexpr int TIER5_POINTS = 100;    // Over 5 minutes
    
    // Bonus multipliers
    constexpr int LIVES_BONUS_MULTIPLIER = 200;
}

// ==========================================
// Lives System
// ==========================================
namespace Lives {
    constexpr int STARTING_LIVES = 6;
}

// ==========================================
// Screen Dimensions
// ==========================================
namespace Screen {
    constexpr int MAX_X = 80;
    constexpr int MAX_Y = 25;
    constexpr int NUM_SCREENS = 3;
}
