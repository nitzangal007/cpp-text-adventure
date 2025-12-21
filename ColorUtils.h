#pragma once
#include <Windows.h>

// ==========================================
// Console Color Utilities
// ==========================================
// Provides optional colored rendering for the game.
// When g_colorsEnabled is false, all color functions are no-ops.

// Global toggle for colored rendering (default: OFF for backward compatibility)
extern bool g_colorsEnabled;

// Available console colors (Windows 16-color palette)
enum class ConsoleColor {
    Default,        // White on black (standard)
    Cyan,           // Walls
    BrightWhite,    // Unbreakable walls
    BrightGreen,    // Springs, Switch ON
    BrightYellow,   // Player 1, Torch, Key
    BrightMagenta,  // Player 2
    Yellow,         // Key (can also be used for torch)
    BrightRed,      // Bomb, Auto-bomb
    DarkRed,        // Switch OFF
    BrightBlue,     // Doors, Riddle
    BrightCyan,     // Hint
    Gray            // Obstacles
};

// Set console foreground color (only if g_colorsEnabled is true)
void setConsoleColor(ConsoleColor color);

// Reset console to default color (only if g_colorsEnabled is true)
void resetColor();

// Get the appropriate color for a given character
ConsoleColor getColorForChar(char ch);
