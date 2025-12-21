#include "Menu.h"
#include "utils.h"
#include "ColorUtils.h"
#include <conio.h>
#include <iostream>
#include <windows.h> 
#include <cstdlib>


void Menu::displayMenu() {
    cls();
    printCentered("=== Main Menu ===", 5);
    printCentered("1. Start Game", 7);
    printCentered("8. Instructions", 8);
    printCentered("9. Exit", 9);
    
    // Color toggle display
    std::string colorStatus = g_colorsEnabled ? "Colors: ON  (press C to toggle)" : "Colors: OFF (press C to toggle)";
    printCentered(colorStatus, 11);
    std::cout << std::endl;
}

Options Menu::getUserChoice() {
    printCentered("Please enter your choice: ", 13);
    char ch = _getch();
    
    // Handle color toggle separately
    if (ch == 'C' || ch == 'c') {
        g_colorsEnabled = !g_colorsEnabled;
        return INVALID;  // Redisplay menu with updated status
    }
    int num = ch - '0';
    switch (num)
    {
    case START_GAME:
        return START_GAME;

    case EXIT_GAME:
        return EXIT_GAME;

    case PRESENT_INSTRUCTIONS:
        return PRESENT_INSTRUCTIONS;

    case INVALID:
    default:

        cls();
        printCentered("Invalid choice. Please try again.", 12);
        std::cout << std::endl << std::endl;
        system("pause");
        return INVALID;
    }
}


void Menu::showInstructions() {
    cls();
    printCentered("========================= INSTRUCTIONS =========================", 1);

    printCentered("Goal: Guide both players to the final room to win.", 3);
    printCentered("Use keys, springs, and teamwork to overcome obstacles.", 4);

    printCentered("--------------------------------------------------------------", 6);
    printCentered("Player Controls", 7);

    printCentered("Player 1:  W=Up  X=Down  A=Left  D=Right  S=Stay  E=Drop", 9);
    printCentered("Player 2:  I=Up  M=Down  J=Left  L=Right  K=Stay  O=Drop", 10);

    printCentered("--------------------------------------------------------------", 12);
    printCentered("Game Elements", 13);

    printCentered("$ : Player 1    & : Player 2    W : Breakable Wall", 15);
    printCentered("# : Spring      K : Key         1-9 : Door", 16);
    printCentered("B : Auto Bomb   H : Hint        X : Indestructible Wall", 17);

    printCentered("--------------------------------------------------------------", 19);
    printCentered("Other Keys", 20);

    printCentered("ESC : Pause/Resume    H : Main Menu (Paused)", 21);
    printCentered("R   : Restart Level", 22);

    printCentered("Press any key to return to the main menu...", 24);

    _getch();

}

Options Menu::runOnce()
{
    while (true)
    {
        displayMenu();
        Options choice = getUserChoice();
        if (choice == PRESENT_INSTRUCTIONS)
        {
            showInstructions();
            if (_kbhit())
            {
                continue; 
            }

        }
		else if (choice == START_GAME || choice == EXIT_GAME)
            return choice;
    }
}

