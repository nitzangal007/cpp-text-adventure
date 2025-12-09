#include "Game.h"
#include <Windows.h>

Game::Game()
	: player1(Player::Id::First, Point(5, 2, 0, 0, '$'), "wdxas", '$'),
	player2(Player::Id::Second, Point(9, 2, 0, 0, '&'), "ilmjk", '&'),
	player1Start(5, 2, 0, 0, '$'),
	player2Start(9, 2, 0, 0, '&'),
	player1ReadyForNextScreen(false),
	player2ReadyForNextScreen(false)
{
	exits[0] = ExitInfo{
		Screens::ScreenId::First,
		Screens::ScreenId::Second,
		Point(43, 20, 0, 0, ' '),
		Point(43, 21, 0, 0, ' '),
		Point(54, 9, 0, 0, '$'),
		Point(26, 9, 0, 0, '&')
	};

	exits[1] = ExitInfo{
		Screens::ScreenId::Second,
		Screens::ScreenId::Final,
		Point(38, 11),
		Point(38, 11),
		Point(),
		Point()
	};
}

// ==========================================
// Public Interface
// ==========================================

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

// ==========================================
// Core Game Control
// ==========================================

void Game::initGame() {
	cls();
	currentScreen.init();
	currentScreen.setCurrentScreen(Screens::ScreenId::Second);
	currentScreen.initFirstScreenSwitches();
	currentScreen.initSecondScreenSwitches();
	player1.reset(exits[0].nextStartP1);
	player2.reset(exits[0].nextStartP2);
	bomb.active = false;
	player1.draw();
	player2.draw();
	drawStatusBar();
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
				if (ch == ESC)
				{
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
				else if (ch == 'R' || ch == 'r') {
					resetCurrentGame();
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
			if (gameOver)
			{
				gameOver = false;
				return;
			}
			render();
		}

		Sleep(100);
	}
}

void Game::resetCurrentGame()
{
	currentScreen.resetCurrent();
	if (currentScreen.isFirstScreen())
	{
		player1Start = Point(5, 2, 0, 0, '$');	
		player2Start = Point(9, 2, 0, 0, '&');
	}
	else if (currentScreen.isSecondScreen())
	{
		player1Start = Point(54, 9, 0, 0, '$');
		player2Start = Point(26, 9, 0, 0, '&');
	}
	player1.reset(player1Start);
	player2.reset(player2Start);
	bomb.active = false;
	bomb.ticksLeft = 0;
	player1ReadyForNextScreen = false;
	player2ReadyForNextScreen = false;
	autoBombs.clear();
}

// ==========================================
// Update & Render
// ==========================================

void Game::updateLogic()
{
	updatePlayerMovement(player1);
	updatePlayerMovement(player2);

	if (!playerIsReadyForNextScreen(player1))
		collectItemIfPossible(player1);

	if (!playerIsReadyForNextScreen(player2))
		collectItemIfPossible(player2);

	currentScreen.updateSwitchStates(player1, player2);
	if (currentScreen.isFirstScreen()) {
		currentScreen.updateFirstScreenGates(player1, player2);
	}
	if (handleAutoBombs())
	{
		resetCurrentGame();
		return;
	}
	
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
	if (currentScreen.getCurrentScreen() != Screens::ScreenId::Final)
	{
		player1.draw();
		player2.draw();
		drawStatusBar();

		// Blink effect: hide bomb character on odd ticks
		if (bomb.active && bomb.ticksLeft <= 5) {
			if (bomb.ticksLeft % 2 != 0) {
				gotoxy(bomb.pos.getX(), bomb.pos.getY());
				std::cout << ' ';
			}
		}

		for (const auto& ab : autoBombs) {
			if (ab.ticksLeft <= 18) {
				if (ab.ticksLeft % 2 != 0) {
					gotoxy(ab.center.getX(), ab.center.getY());
					std::cout << ' ';
				}
			}
		}

		const Point& p1Pos = player1.getPosition();
		const Point& p2Pos = player2.getPosition();

		if (currentScreen.isHint(p1Pos) || currentScreen.isHint(p2Pos))
		{
			currentScreen.printHint();   
		}
		else
		{
			currentScreen.clearHint();   
		}
	}
}

void Game::drawStatusBar()
{
	gotoxy(0, Screens::MAX_Y-4); 
	std::cout << "Player 1 holding: [" << player1.getHeldItem() << "]";

	gotoxy(0, Screens::MAX_Y-1); 
	std::cout << "Player 2 holding: [" << player2.getHeldItem() << "]";
}

// ==========================================
// Player Logic
// ==========================================

void Game::updatePlayerMovement(Player& player)
{
	if (playerIsReadyForNextScreen(player))
	{
		return;
	}
	Point nextPos = player.getPosition();
	nextPos.move();

	Player& other = getOtherPlayer(player);
	Point otherPos = other.getPosition();

	// Block movement if walking into the other player
	if (nextPos.getX() == otherPos.getX() &&
		nextPos.getY() == otherPos.getY() && !isExitWaitPosition(nextPos))
	{
		player.stop();
		return;
	}

	if (currentScreen.isObstacle(nextPos))
	{
		bool Pushable = currentScreen.tryPushObstacle(nextPos, player, getOtherPlayer(player));
		if (Pushable)
		{
			currentScreen.makePassage(nextPos);
			player.move();
			return;
		}
		else
		{
			return;
		}
	}
		
	if (currentScreen.isDoor(nextPos) && player.hasKey())
	{
		currentScreen.makePassage(nextPos);
		player.removeHeldItem();
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

void Game::collectItemIfPossible(Player& player)
{
	const Point& pos = player.getPosition();

	if (player.hasKey() || player.hasBomb() || player.hasTorch())
		return;

	if (currentScreen.isKey(pos))
	{
		player.collectKey();
		currentScreen.makePassage(pos);
	}
	else if (currentScreen.isBomb(pos))
	{
		if (bomb.active &&
			pos.getX() == bomb.pos.getX() &&
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

Player& Game::getOtherPlayer(const Player& p)
{
	if (p.getId() == Player::Id::First)
		return player2;
	else
		return player1;
}

// ==========================================
// Bomb Logic
// ==========================================

void Game::tryPlaceBomb(Player& player)
{
	if(bomb.active)
		return;
	else if(!player.hasBomb())
		return;
	else
	{
		bomb.active = true;
		bomb.pos = player.getPosition();
		bomb.ticksLeft = 5; 
		player.removeHeldItem();
	}
	
	int x = bomb.pos.getX();
	int y = bomb.pos.getY();
	currentScreen.placeBombAt(x, y);
}

void Game::explodeBomb()
{
	if (!bomb.active)
		return;

	const int RADIUS = 3;
	const int R2 = RADIUS * RADIUS;
	Point center = bomb.pos;
	
	currentScreen.clearExplosionArea(center, RADIUS);

	bool p1Dead = isPlayerInExplosion(player1, bomb.pos, R2);
	bool p2Dead = isPlayerInExplosion(player2, bomb.pos, R2);

	if (p1Dead || p2Dead)
	{
		resetCurrentGame();
		return;
	}
	
	bomb.active = false;
	bomb.ticksLeft = 0;
}

bool Game::handleAutoBombs()
{
	std::vector<Point> centers;
	currentScreen.collectPendingAutoBombs(centers);

	constexpr int AUTO_BOMB_DELAY = 21;

	for (const Point& c : centers)
	{
		AutoBomb ab;
		ab.center = c;
		ab.ticksLeft = AUTO_BOMB_DELAY;
		autoBombs.push_back(ab);
	}

	if (autoBombs.empty())
		return false;

	const int RADIUS = 3;
	const int R2 = RADIUS * RADIUS;

	bool someoneDied = false;

	for (size_t i = 0; i < autoBombs.size(); )
	{
		AutoBomb& ab = autoBombs[i];

		if (ab.ticksLeft > 0)
		{
			--ab.ticksLeft;
			++i;
			continue;
		}

		currentScreen.clearExplosionArea(ab.center, RADIUS);

		bool p1Dead = isPlayerInExplosion(player1, ab.center, R2);
		bool p2Dead = isPlayerInExplosion(player2, ab.center, R2);

		if (p1Dead || p2Dead)
			someoneDied = true;
		
		currentScreen.setCharAt(ab.center, Screens::EMPTY_SPACE);

		// Erase exploded bomb; don't increment i to not skip next element
		autoBombs.erase(autoBombs.begin() + i);
	}
	return someoneDied;
}







void Game::initGame() {
	// Initialize game state, load resources, etc.
	cls();
	currentScreen.init();
	currentScreen.setCurrentScreen(Screens::ScreenId::First);
	currentScreen.initFirstScreenSwitches();
	currentScreen.initSecondScreenSwitches();
	player1.reset(player1Start);
	player2.reset(player2Start);
	bomb.active = false;
	player1.draw();
	player2.draw();
	drawStatusBar();

}
void Game::updateLogic()
bool Game::isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared)
{
	Point p = player.getPosition();
	int dx = p.getX() - center.getX();
	int dy = p.getY() - center.getY();

	return (dx * dx + dy * dy <= radiusSquared);
}

// ==========================================
// Screen Transition Logic
// ==========================================

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

		if (exit.to == Screens::ScreenId::Final)
		{
			currentScreen.drawCurrent();
			gotoxy(18, 18);
			std::cout << "Press any key to return to the main menu...";
			_getch();      
			gameOver = true;
		}
		else
		{
			player1.draw();
			player2.draw();
			drawStatusBar();
		}

		return;
	}
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
	Screens::ScreenId current = currentScreen.getCurrentScreen();

	for (const ExitInfo& exit : exits)
	{
		if (exit.from != current)
			continue;

		if (exit.waitPos.getX() == p.getX() &&
			exit.waitPos.getY() == p.getY())
		{
			return true;
		}
	}
	return false;
}
