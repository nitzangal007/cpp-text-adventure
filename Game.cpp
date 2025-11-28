#include "Game.h"
#include <Windows.h>
void Game::initGame() {
	// Initialize game state, load resources, etc.
	int screen = 1;

	cls();
	player1.draw();
	player2.draw();
}

void Game::runGame() {
	const char ESC = 27;
	bool paused = false;
	bool running = true;
	while (running)
	{
		if (_kbhit())
		{
			char ch = _getch();
			if (!paused)
			{
				if (ch == ESC)
				{
					paused = true;
					cls();
					printCentered("Game paused,", 8);
					printCentered("press ESC again to continue or H to go back to the main menu,", 9);
				}
				else
				{
					player1.handleKeyPress(ch);
					player2.handleKeyPress(ch);
				}

			}
			else {
				if (ch == ESC)
				{
					cls();
					paused = false;
				}
				else if (ch == 'H' || ch == 'h')
				{
					return;
				}
			}


		}
		if (!paused)
		{
			player1.move();
			player2.move();


			player1.draw();
			player2.draw();
			Sleep(100);
		}


	}




}
