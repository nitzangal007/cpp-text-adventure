#include "Game.h"
#include <Windows.h>
void Game::updatePlayerMovement(Player& player)
{
	if (playerIsReadyForNextScreen(player))
	{
		return;
	}
	Point nextPos = player.getPosition();
	nextPos.move();
	if (currentScreen.isDoor(nextPos) && player.hasKey())
	{
		currentScreen.makePassage(nextPos);
		player.removeKey();
		player.move();
	}
	else if (isExitWaitPosition(nextPos))
	{
		player.move();
		player.stop();
		if (player.getId() == Player::Id::First)
		{
			player1ReadyForNextScreen = true;
		}
		else
		{
			player2ReadyForNextScreen = true;
		}
		return;
	}
	else if (currentScreen.isFreeCellForPlayer(nextPos))
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
		if (bomb.active && pos.getX() == bomb.pos.getX() &&
			pos.getY() == bomb.pos.getY())
		{
			return;
		}
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

void Game::tryPlaceBomb(Player& player)
{
	if(bomb.active)
		return; // already a bomb active
	else if(!player.hasBomb())
		return; // player has no bomb to place
	
	else
	{
		bomb.active = true;
		bomb.pos = player.getPosition();
		bomb.ticksLeft = 5; 
		player.removeBomb();
		}
	
	int x = bomb.pos.getX();
	int y = bomb.pos.getY();
	currentScreen.placeBombAt(x, y);

}

void Game::explodeBomb()
{

	if (!bomb.active)
		return;

	const int RADIUS = 2;
	const int R2 = RADIUS * RADIUS;
	Point center = bomb.pos;

	
	currentScreen.clearExplosionArea(center, RADIUS);


	if (isPlayerInExplosion(player1, center, R2))
	{
		player1.reset(player1Start);
	}


	if (isPlayerInExplosion(player2, center, R2))
	{
		player2.reset(player2Start);
	}

	
	bomb.active = false;
	bomb.ticksLeft = 0;
}

bool Game::isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared)
{
	Point p = player.getPosition();
	int dx = p.getX() - center.getX();
	int dy = p.getY() - center.getY();

	return (dx * dx + dy * dy <= radiusSquared);
}

bool Game::playerIsReadyForNextScreen(const Player& player) const
{
	if (Player::Id::First == player.getId())
	{
		return player1ReadyForNextScreen;
	}
	else
	{
		return player2ReadyForNextScreen;
	}
}

bool Game::isExitWaitPosition(const Point& p) const
{
	for (const ExitInfo& exit : exits)
	{
		if (exit.waitPos.getX() == p.getX() &&
			exit.waitPos.getY() == p.getY())
		{
			return true;
		}
	}
	return false;
	
}

void Game::tryAdvanceToNextScreen()
{
	for (const ExitInfo& exit : exits)
	{
		if (currentScreen.getCurrentScreen() != exit.from)
			continue;
		if (!playerIsReadyForNextScreen(player1) ||
			!playerIsReadyForNextScreen(player2))
		{
			return;
		}

		currentScreen.setCurrentScreen(exit.to);

		player1.reset(exit.nextStartP1);
		player2.reset(exit.nextStartP2);

		player1ReadyForNextScreen = false;
		player2ReadyForNextScreen = false;

		cls();
		currentScreen.drawCurrent();
		player1.draw();
		player2.draw();
		drawStatusBar();
		return;
	}
}



void Game::initGame() {
	// Initialize game state, load resources, etc.
	cls();
	currentScreen.init();
	player1.reset(player1Start);
	player2.reset(player2Start);
	bomb.active = false;
	player1.draw();
	player2.draw();
	drawStatusBar();
	
}
void Game::updateLogic()
{
	updatePlayerMovement(player1);
	updatePlayerMovement(player2);

	if (!playerIsReadyForNextScreen(player1))
		collectItemIfPossible(player1);

	if (!playerIsReadyForNextScreen(player2))
		collectItemIfPossible(player2);

	updateSwitchRows();

	if (bomb.active)
	{
		bomb.ticksLeft--;
		if (bomb.ticksLeft <= 0)
		{
			explodeBomb();
		}
	}
	tryAdvanceToNextScreen();
}
void Game::render()
{
	currentScreen.drawCurrent();
	player1.draw();
	player2.draw();
	drawStatusBar();
}

void Game::run()
{
	Menu menu;
	bool done = false;
	while (!done) {
		
		Options choice = menu.runOnce();

		switch (choice) {
		case START_GAME:

			initGame();
			runGame();     
			break;

		case PRESENT_INSTRUCTIONS:
			menu.showInstructions();  
			break;

		case EXIT_GAME:
			done = true;  
			break;


		}
	}
}
void Game::runGame()
{
	const char ESC = 27;
	bool paused = false;
	bool running = true;

	cls();
	render();
	


	while (running)
	{
		if (_kbhit())
		{
			char ch = _getch();

			if (!paused)
			{
				// מצב משחק רגיל
				if (ch == ESC)
				{
					// נכנסים ל-PAUSE
					cls();
					paused = true;
					printCentered("Game paused,", 8);
					printCentered("press ESC again to continue or H to go back to the main menu,", 9);
				}
				else if (ch == 'E' || ch == 'e') {
					tryPlaceBomb(player1);
				}
				else if (ch == 'O' || ch == 'o') {
					tryPlaceBomb(player2);
				}
				else
				{
					player1.handleKeyPress(ch);
					player2.handleKeyPress(ch);
				}
			}
			else
			{
				
				if (ch == ESC)
				{
					
					paused = false;

				
					cls();
					render();
				}
				else if (ch == 'h' || ch == 'H')
				{
					
					return;  
				}
			}
		}

		if (!paused)
		{
			updateLogic();
			render();
		}

		Sleep(100);
	}
}





