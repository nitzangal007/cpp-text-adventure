#include <iostream>                                        // utils.cpp
#include <windows.h>                                       // We took the code from class
#include <cstdlib>

#include "utils.h"

// External silent mode flag (defined in GameFileInput.cpp)
extern bool g_silentMode;

void gotoxy(int x, int y) {
    // Skip cursor movement in silent mode
    if (g_silentMode) return;
    
    std::cout.flush();
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    // Skip in silent mode
    if (g_silentMode) return;
    
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE; // Set to TRUE to make it visible
    SetConsoleCursorInfo(hStdOut, &curInfo);
}

void cls() {
    // Skip screen clear in silent mode
    if (g_silentMode) return;
    
    system("cls");
}

void printCentered(const std::string& text, int y)     // we used chatGPT to generate this function
{
    // Skip in silent mode
    if (g_silentMode) return;
    
    const int screenWidth = 80;  
    int x = (screenWidth - static_cast<int>(text.size())) / 2;
    if (x < 0) x = 0; 

    gotoxy(x, y);
    std::cout << text;
}

