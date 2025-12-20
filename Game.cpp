#include "Game.h"
#include <Windows.h>
#include <chrono>

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
		Point(25, 9, 0, 0, '&')
	};

	exits[1] = ExitInfo{
		Screens::ScreenId::Second,
		Screens::ScreenId::Final,
		Point(38, 13),
		Point(38, 13),
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
	player1Start = Point(5, 2, 0, 0, '$');
	player2Start = Point(9, 2, 0, 0, '&');
	currentScreen.init();
	
	// Check if screen loading failed
	if (currentScreen.hasLoadingError())
	{
		// Display error message and return to menu
		cls();
		gotoxy(5, 10);
		std::cout << "ERROR: " << currentScreen.getLoadingError();
		gotoxy(5, 12);
		std::cout << "Make sure adv-world*.screen files are in the game directory.";
		gotoxy(5, 14);
		std::cout << "Press any key to return to menu...";
		_getch();
		gameOver = true;  // Signal to return to menu
		return;
	}
	
	currentScreen.setCurrentScreen(Screens::ScreenId::First);
	player1.reset(player1Start);
	player2.reset(player2Start);
	bomb.active = false;
	
	// Reset lives & score for new game
	lives = 6;
	score = 0;
	levelStartTime = getCurrentTimeSeconds();
	
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
					printCentered("       Game Paused        ", 8);
					printCentered("Press ESC to continue or H for Menu", 9);
				}
				else if (ch == 'E' || ch == 'e') {
					if (player1.hasTorch())
						dropTorch(player1);
					else
						tryPlaceBomb(player1);
				}
				else if (ch == 'O' || ch == 'o') {
					if (player2.hasTorch())
						dropTorch(player2);
					else
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
	// Decrement life (may trigger game over)
	decrementLife();
	if (gameOver)
		return;
	
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
	// Use torch-aware drawing for dark screen support
	currentScreen.drawCurrentWithTorch(player1, player2);
	if (currentScreen.getCurrentScreen() != Screens::ScreenId::Final)
	{
		player1.draw();
		player2.draw();
		drawStatusBar();

																
		if (bomb.active && bomb.ticksLeft <= 5) {					 // we used Gemini for this block
			if (bomb.ticksLeft % 2 != 0) {
				gotoxy(bomb.pos.getX(), bomb.pos.getY());
				std::cout << ' ';
			}
		}

		for (const auto& ab : autoBombs) {						    // we used Gemini for this block
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
	// Get legend Y position from current screen (where 'L' was found in file)
	int legendStart = currentScreen.getLegendY();
	
	// Line 1: Player items, lives, score
	gotoxy(0, legendStart); 
	std::cout << "P1:[" << player1.getHeldItem() << "]  P2:[" << player2.getHeldItem() << "]  ";
	std::cout << "Lives:" << lives << "  Score:" << score << "          ";

	// Line 2: Controls hint
	gotoxy(0, legendStart + 1); 
	std::cout << "ESC:Pause  R:Restart  E/O:Drop                    ";
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

	if (currentScreen.isObstacle(nextPos))								// we used chatGPT for this block
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
		decrementLife();
		if (!gameOver)
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
		return;
	}
	
	bomb.active = false;
	bomb.ticksLeft = 0;
}

bool Game::handleAutoBombs()							//We used chatGPT for this function
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

		autoBombs.erase(autoBombs.begin() + i);
	}
	return someoneDied;
}

bool Game::isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared)
{
	Point p = player.getPosition();
	int dx = p.getX() - center.getX();
	int dy = p.getY() - center.getY();

	return (dx * dx + dy * dy <= radiusSquared);
}

void Game::dropTorch(Player& player)
{
	if (!player.hasTorch())
		return;
	
	// Place torch back on the board at player's current position
	Point pos = player.getPosition();
	currentScreen.setCharAt(pos, Screens::TORCH);
	player.removeHeldItem();
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

		// Add score for completing level
		addLevelCompletionScore();
		
		// Extra life for completing level
		lives++;

		currentScreen.setCurrentScreen(exit.to);

		player1.reset(exit.nextStartP1);
		player2.reset(exit.nextStartP2);

		player1ReadyForNextScreen = false;
		player2ReadyForNextScreen = false;

		cls();
		currentScreen.drawCurrent();

		if (exit.to == Screens::ScreenId::Final)
		{
			// Victory screen - show final score on FINAL_SCREEN_TEMPLATE
			currentScreen.drawCurrent();
			gotoxy(30, 17);
			std::cout << "Final Score: " << score;
			gotoxy(18, 19);
			std::cout << "Congratulations! Press any key to return to menu...";
			_getch();      
			gameOver = true;
		}
		else
		{
			// Reset timer for next level
			levelStartTime = getCurrentTimeSeconds();
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

// ==========================================
// Lives & Score System
// ==========================================

void Game::decrementLife()
{
	lives--;
	if (lives <= 0)
	{
		showGameOverScreen();
		gameOver = true;
	}
}

void Game::addLevelCompletionScore()
{
	// Time-based score (faster = more points)
	int elapsedSeconds = getCurrentTimeSeconds() - levelStartTime;
	int timeScore;
	if (elapsedSeconds <= 60)        // Under 1 minute
		timeScore = 2000;
	else if (elapsedSeconds <= 120)  // 1-2 minutes
		timeScore = 1500;
	else if (elapsedSeconds <= 180)  // 2-3 minutes
		timeScore = 1000;
	else if (elapsedSeconds <= 300)  // 3-5 minutes
		timeScore = 500;
	else                             // Over 5 minutes
		timeScore = 100;

	// Lives bonus (remaining lives x 200)
	int livesBonus = lives * 200;

	score += timeScore + livesBonus;
}

void Game::showGameOverScreen()
{
	cls();
	// Use FINAL_SCREEN_TEMPLATE (which shows "GAME OVER")
	currentScreen.setCurrentScreen(Screens::ScreenId::Final);
	currentScreen.drawCurrent();
	
	gotoxy(30, 17);
	std::cout << "Final Score: " << score;
	gotoxy(22, 19);
	std::cout << "Press any key to return to the main menu...";
	_getch();
}

int Game::getCurrentTimeSeconds()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}
