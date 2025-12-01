#include "Game.h"
#include <Windows.h>
void Game::updatePlayerMovement(Player& player)
{
	Point nextPos = player.getPosition();
	nextPos.move();
	if (currentScreen.isDoor(nextPos) && player.hasKey())
	{
		currentScreen.makePassage(nextPos);
		player.removeKey();
		player.move();
	}
	if (currentScreen.isFreeCellForPlayer(nextPos))
	{
		player.move();
		
	}
	else
	{
		player.stop();
	}
}
void Game::updateSwitchRows()
{
	for (int y = 0; y < Screens::MAX_Y; ++y)
	{
		if (!currentScreen.hasSwitchInRow(y))
			continue;
		bool rowOpen = false;
		auto checkPlayer = [&](const Player& player) {
			const Point& pos = player.getPosition();
			if (pos.getY() == y && currentScreen.isSwitch(pos))
			{
				rowOpen = true;
			}
		};
		checkPlayer(player1);
		checkPlayer(player2);
		currentScreen.setRowWallsRaised(y, rowOpen);
		
	}
}
void Game::collectItemIfPossible(Player& player)
{
	const Point& pos = player.getPosition();
	if (currentScreen.isKey(pos))
	{
		player.collectKey();
		currentScreen.makePassage(pos);
	}
	else if (currentScreen.isBomb(pos))
	{
		player.collectBomb();
		currentScreen.makePassage(pos);
	}
	else if (currentScreen.isTorch(pos))
	{
		player.collectTorch();
		currentScreen.makePassage(pos);
	}
}
void Game::drawStatusBar()
{
	gotoxy(0, Screens::MAX_Y-4); 

	char key = player1.hasKey() ? Screens::KEY : Screens::EMPTY_SPACE;
	char bomb = player1.hasBomb() ? Screens::BOMB : Screens::EMPTY_SPACE;
	char torch = player1.hasTorch() ? Screens::TORCH : Screens::EMPTY_SPACE;

	char key2 = player2.hasKey() ? Screens::KEY : Screens::EMPTY_SPACE;
	char bomb2 = player2.hasBomb() ? Screens::BOMB : Screens::EMPTY_SPACE;
	char torch2 = player2.hasTorch() ? Screens::TORCH : Screens::EMPTY_SPACE;

	std::cout << "Player 1: Key[" << key << "] Bomb[" << bomb << "] Torch[" << torch << "]" << std::endl;

	gotoxy(0, Screens::MAX_Y-1); 
	std::cout << "Player 2: Key[" << key2 << "] Bomb[" << bomb2 << "] Torch[" << torch2 << "]";
}

void Game::initGame() {
	// Initialize game state, load resources, etc.
	cls();
	currentScreen.init();
	player1.draw();
	player2.draw();
	drawStatusBar();
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
			updatePlayerMovement(player1);
			updatePlayerMovement(player2);

			collectItemIfPossible(player1);
			collectItemIfPossible(player2);

			updateSwitchRows();      

			currentScreen.drawCurrent();
			drawStatusBar();
			player1.draw();
			player2.draw();

			Sleep(100);
		}


	}




}
