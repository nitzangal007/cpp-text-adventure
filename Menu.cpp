#include "Menu.h"
#include "utils.h"
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
    std::cout << std::endl;
}

Options Menu::getUserChoice() {
    printCentered("Please enter your choice: ", 11);
    char ch = _getch();
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

    printCentered("Goal: Guide both players through the adventure world.", 3);
    printCentered("Use their movement and abilities to overcome obstacles,", 4);
    printCentered("activate springs, collect keys, and open doors.", 5);
    printCentered("Only when both players reach the final room, you win.", 6);

    printCentered("--------------------------------------------------------------", 8);
    printCentered("Player Controls", 9);

    printCentered("Player 1:  W=Up  X=Down  A=Left  D=Right  S=Stay  E=Drop", 11);
    printCentered("Player 2:  I=Up  M=Down  J=Left  L=Right  K=Stay  O=Drop", 12);

    printCentered("--------------------------------------------------------------", 14);
    printCentered("Game Elements", 15);

    printCentered("$ : Player 1    & : Player 2    W : Wall", 17);
    printCentered("# : Spring      K : Key         1-9 : Door", 18);

    printCentered("--------------------------------------------------------------", 20);
    printCentered("Other Keys", 21);

    printCentered("ESC : Pause game. While paused: ESC=resume, H=main menu.", 22);

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

