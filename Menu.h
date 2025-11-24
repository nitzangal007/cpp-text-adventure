#pragma once

#include <iostream>

enum Options { START_GAME = 1, PRESENT_INSTRUCTIONS = 8, EXIT_GAME = 9, INVALID };
class Menu
{
	public:
		void displayMenu();
		Options getUserChoice();
		void showInstructions();
		Options runOnce();
};

