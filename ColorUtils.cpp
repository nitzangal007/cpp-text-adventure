#include "ColorUtils.h"

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
    case 'W':  return ConsoleColor::Cyan;           // Wall
    case 'X':  return ConsoleColor::BrightWhite;    // Unbreakable wall
    case '#':  return ConsoleColor::BrightGreen;    // Spring

    // Players
    case '$':  return ConsoleColor::BrightYellow;   // Player 1
    case '&':  return ConsoleColor::BrightMagenta;  // Player 2

    // Items
    case 'K':  return ConsoleColor::Yellow;         // Key
    case '!':  return ConsoleColor::BrightYellow;   // Torch
    case '@':  return ConsoleColor::BrightRed;      // Bomb

    // Logic/Puzzles
    case '/':  return ConsoleColor::BrightGreen;    // Switch ON
    case '\\': return ConsoleColor::DarkRed;        // Switch OFF
    case '?':  return ConsoleColor::BrightBlue;     // Riddle
    case 'H':  return ConsoleColor::BrightCyan;     // Hint

    // Doors (1-9)
    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
        return ConsoleColor::BrightBlue;

    // Other
    case '*':  return ConsoleColor::Gray;           // Obstacle
    case 'B':  return ConsoleColor::BrightRed;      // Auto-bomb

    default:
        return ConsoleColor::Default;
    }
}
