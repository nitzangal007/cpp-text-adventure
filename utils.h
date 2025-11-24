#pragma once
#include <string>

void gotoxy(int x, int y);
void hideCursor();
void cls();
void printCentered(const std::string& text, int y); // we used chatGPT to generate this function

