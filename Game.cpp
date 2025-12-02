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
		if (bomb.active && pos.getX() == bomb.pos.getX() &&
			pos.getY() == bomb.pos.getY())
		{
			// can't collect an active bomb
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
		bomb.ticksLeft = 5; // for example, 5 ticks until explosion
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

	// 1. הורסים את האזור במפה
	currentScreen.clearExplosionArea(center, RADIUS);

	// 2. אם שחקן 1 בתוך הפיצוץ - מאפסים אותו
	if (isPlayerInExplosion(player1, center, R2))
	{
		player1.reset(player1Start);
	}

	// 3. כנ"ל לגבי שחקן 2
	if (isPlayerInExplosion(player2, center, R2))
	{
		player2.reset(player2Start);
	}

	// 4. מכבים את הבומבה
	
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

void Game::run()
{
	Menu menu;
	bool done = false;
	while (!done) {
		
		Options choice = menu.runOnce();

		switch (choice) {
		case START_GAME:

			initGame();
			runGame();      // זה המשחק הבודד – מה שיש לכם היום
			break;

		case PRESENT_INSTRUCTIONS:
			menu.showInstructions();   // או איך שקראת לפונקציה
			break;

		case EXIT_GAME:
			done = true;    // יוצאים מהלולאה → חוזרים למיין → התוכנית נגמרת
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
	drawStatusBar();
	player1.draw();
	player2.draw();


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
				// מצב PAUSE
				if (ch == ESC)
				{
					// ESC שוב → ממשיכים מהמשחק
					paused = false;

					// מוחקים את הודעת ה-PAUSE ומחזירים את המסך כמו שהיה
					cls();
					currentScreen.drawCurrent();
					drawStatusBar();
					player1.draw();
					player2.draw();
				}
				else if (ch == 'h' || ch == 'H')
				{
					// חוזרים למניו – המשחק הזה נגמר
					return;   // יוצא מ-runGame וחוזר ל-Game::run → תפריט
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
		

			if (bomb.active)
			{
				bomb.ticksLeft--;
				if (bomb.ticksLeft <= 0)
				{
					explodeBomb();
				}
			}
			currentScreen.drawCurrent();
			player1.draw();
			player2.draw();
			drawStatusBar();
		}

		Sleep(100);
	}
}



