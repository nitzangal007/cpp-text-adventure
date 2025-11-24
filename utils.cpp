#include <iostream>
#include <windows.h> // Required for Windows API console functions
#include <cstdlib>

#include "utils.h"

void gotoxy(int x, int y) {
    std::cout.flush();
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE; // Set to TRUE to make it visible
    SetConsoleCursorInfo(hStdOut, &curInfo);
}

void cls() {
    system("cls");
}

void printCentered(const std::string& text, int y)
{
    const int screenWidth = 80;  
    int x = (screenWidth - static_cast<int>(text.size())) / 2;
    if (x < 0) x = 0; 

    gotoxy(x, y);
    std::cout << text;
}

