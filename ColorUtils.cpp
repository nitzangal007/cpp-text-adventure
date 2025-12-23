#include "ColorUtils.h"
#include "GameConstants.h"

// Global color toggle - OFF by default for backward compatibility
bool g_colorsEnabled = false;

// Windows console color codes
static const WORD DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

void setConsoleColor(ConsoleColor color)
{
    if (!g_colorsEnabled)
        return;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD attr = DEFAULT_COLOR;

    switch (color) {
    case ConsoleColor::Default:
        attr = DEFAULT_COLOR;
        break;
    case ConsoleColor::Cyan:
        attr = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case ConsoleColor::BrightWhite:
        attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::BrightGreen:
        attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::BrightYellow:
        attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::BrightMagenta:
        attr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::Yellow:
        attr = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
    case ConsoleColor::BrightRed:
        attr = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::DarkRed:
        attr = FOREGROUND_RED;
        break;
    case ConsoleColor::BrightBlue:
        attr = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::BrightCyan:
        attr = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case ConsoleColor::Gray:
        attr = FOREGROUND_INTENSITY;
        break;
    }

    SetConsoleTextAttribute(hConsole, attr);
}

void resetColor()
{
    if (!g_colorsEnabled)
        return;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, DEFAULT_COLOR);
}

ConsoleColor getColorForChar(char ch)
{
    switch (ch) {
    // Geometry
    case Tiles::WALL:  return ConsoleColor::Cyan;           // Wall
    case Tiles::UNBREAKABLE_WALL:  return ConsoleColor::BrightWhite;    // Unbreakable wall
    case Tiles::SPRING:  return ConsoleColor::BrightGreen;    // Spring

    // Players
    case Players::PLAYER1_SYMBOL:  return ConsoleColor::BrightYellow;   // Player 1
    case Players::PLAYER2_SYMBOL:  return ConsoleColor::BrightMagenta;  // Player 2

    // Items
    case Tiles::KEY:  return ConsoleColor::Yellow;         // Key
    case Tiles::TORCH:  return ConsoleColor::BrightYellow;   // Torch
    case Tiles::BOMB:  return ConsoleColor::BrightRed;      // Bomb

    // Logic/Puzzles
    case Tiles::SWITCH_ON:  return ConsoleColor::BrightGreen;    // Switch ON
    case Tiles::SWITCH_OFF: return ConsoleColor::DarkRed;        // Switch OFF
    case Tiles::RIDDLE:  return ConsoleColor::BrightBlue;     // Riddle
    case Tiles::HINT:  return ConsoleColor::BrightCyan;     // Hint

    // Doors (1-9)
    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
        return ConsoleColor::BrightBlue;

    // Other
    case Tiles::OBSTACLE:  return ConsoleColor::Gray;           // Obstacle
    case Tiles::AUTO_BOMB:  return ConsoleColor::BrightRed;      // Auto-bomb

    default:
        return ConsoleColor::Default;
    }
}
