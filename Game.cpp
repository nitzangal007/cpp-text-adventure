#include "Game.h"
#include "ColorUtils.h"
#include "GameConstants.h"
#include "Torch.h"
#include "Key.h"
#include <Windows.h>
#include <chrono>

Game::Game()
	: player1(Player::Id::First, Point(5, 2, 0, 0, Players::PLAYER1_SYMBOL), "wdxas", Players::PLAYER1_SYMBOL),
	player2(Player::Id::Second, Point(9, 2, 0, 0, Players::PLAYER2_SYMBOL), "ilmjk", Players::PLAYER2_SYMBOL),
	player1Start(5, 2, 0, 0, Players::PLAYER1_SYMBOL),
	player2Start(9, 2, 0, 0, Players::PLAYER2_SYMBOL),
	player1ReadyForNextScreen(false),
	player2ReadyForNextScreen(false)
{
	exits[0] = ExitInfo{
		Screens::ScreenId::First,
		Screens::ScreenId::Second,
		Point(43, 20, 0, 0, Tiles::EMPTY_SPACE),
		Point(43, 21, 0, 0, Tiles::EMPTY_SPACE),
		Point(54, 9, 0, 0, Players::PLAYER1_SYMBOL),
		Point(25, 9, 0, 0, Players::PLAYER2_SYMBOL)
	};

	exits[1] = ExitInfo{
		Screens::ScreenId::Second,
		Screens::ScreenId::Third,
		Point(38, 13),
		Point(38, 13),
		Point(36,2,0,0,Players::PLAYER1_SYMBOL),
		Point(43,2,0,0,Players::PLAYER2_SYMBOL)
	};

	exits[2] = ExitInfo{
		Screens::ScreenId::Third,
		Screens::ScreenId::Final,
		Point(41, 20),
		Point(41, 20),
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
	player1Start = Point(5, 2, 0, 0, Players::PLAYER1_SYMBOL);
	player2Start = Point(9, 2, 0, 0, Players::PLAYER2_SYMBOL);
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
	
	// Reset bomb for new game
	bomb = Bomb();
	
	// Reset lives & score for new game
	lives = Lives::STARTING_LIVES;
	score = 0;
	gameTimeMs = 0;
	
	// Initialize M-trap timer (not really needed with gameTimeMs, but good to reset)
	mTrapTimerStart = std::chrono::steady_clock::now();
	mTrapVisible = true;
	
	// Reset pause tracking (deprecated)
	accumulatedPauseMs = 0;
	accumulatedPauseSec = 0;
	
	// Reset story flags for new game
	shownStory1 = false;
	shownStory2 = false;
	
	player1.draw();
	player2.draw();
	drawStatusBar();
	
	// Show STORY_1 at start of game (only once, and not in load mode)
	if (!shownStory1 && shouldShowOverlay()) {
		render();
		showStoryOverlay(1);
		shownStory1 = true;
	}
}

void Game::runGame()
{
	const char ESC = Keys::ESC;
	bool paused = false;
	bool running = true;

	// Reset iteration counter for new game
	iteration = 0;

	cls();
	doRender();
	
	while (running)
	{
		// Check if we should finish automatically (for load mode)
		if (shouldFinishGame()) {
			return;
		}

		if (hasInput())
		{
			char ch = getNextInput();

			if (!paused)
			{
				// Track input for recording
				char p1Key = '\0';
				char p2Key = '\0';
				
				// ========================================
				// Room 3 Boss Input Handling
				// ========================================
				if (currentScreen.isThirdScreen() && room3Boss.isActive())
				{
					// Handle V submission and other boss input
					if (room3Boss.handleInput(ch, currentScreen, player1, player2))
					{
						// Apply any pending penalties - record each life lost
						int penalty = room3Boss.getLifePenalty();
						for (int i = 0; i < penalty; ++i) {
							lives--;
							if (lives <= 0) {
								lives = 0;
								showGameOverScreen();
								recordGameEnded(score);
								gameOver = true;
								return;
							}
							recordLostLife();
						}
						if (room3Boss.getScorePenalty() > 0) {
							score = (score >= room3Boss.getScorePenalty()) 
								? score - room3Boss.getScorePenalty() : 0;
						}
							room3Boss.clearPenalties();
						
						// Check if a task was completed and record it
						if (room3Boss.getCompletedTaskNumber() > 0) {
							recordBossTaskComplete(room3Boss.getCompletedTaskNumber());
							room3Boss.clearCompletedTask();
						}
						
						// Record the boss input as a step
						// Boss input is handled specially - consider all as player1 input
						recordStep(ch, '\0');
						continue;
					}
				}
				
				if (ch == ESC)
				{
					cls();
					paused = true;
					// Record when pause started (for timer freezing)
					pauseStartTime = std::chrono::steady_clock::now();
					printCentered("       Game Paused        ", 8);
					printCentered("Press ESC to continue or H for Menu", 9);
				}
				else if (ch == 'E' || ch == 'e') {
					if (player1.hasTorch())
						dropTorch(player1);
					else
						tryPlaceBomb(player1);
					p1Key = ch;
				}
				else if (ch == 'O' || ch == 'o') {
					if (player2.hasTorch())
						dropTorch(player2);
					else
						tryPlaceBomb(player2);
					p2Key = ch;
				}
				else if (ch == 'R' || ch == 'r') {
					// Disable R during active boss fight
					if (currentScreen.isThirdScreen() && room3Boss.isRestartDisabled()) {
						// R is disabled during boss - do nothing
					}
					else {
						// Record R key press before reset (lostLife will be recorded in decrementLife)
						recordStep(ch, '\0');
						resetCurrentGame();
					}
				}
				else
				{
					// Check which player this input belongs to
					if (player1.handleKeyPress(ch)) {
						p1Key = ch;
					}
					if (player2.handleKeyPress(ch)) {
						p2Key = ch;
					}
				}
				
				// Record step if any player had input
				if (p1Key != '\0' || p2Key != '\0') {
					recordStep(p1Key, p2Key);
				}
			}
			else
			{
				if (ch == ESC)
				{
					// Calculate pause duration and add to accumulators
					auto now = std::chrono::steady_clock::now();
					auto pauseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
						now - pauseStartTime
					).count();
					accumulatedPauseMs += pauseDuration;
					accumulatedPauseSec += pauseDuration / 1000;
					
					paused = false;
					cls();
					doRender();
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
			doRender();
			++iteration;  // Increment iteration counter
			gameTimeMs += Timing::GAME_TICK_MS; // Deterministic time step
		}

		Sleep(getSleepDuration());
	}
}


void Game::resetCurrentGame()
{
	// Decrement life (may trigger game over)
	decrementLife();
	if (gameOver)
		return;
	
	currentScreen.resetCurrent();
	currentScreen.resetRiddlesForCurrentScreen();
	if (currentScreen.isFirstScreen())
	{
		player1Start = Point(5, 2, 0, 0, Players::PLAYER1_SYMBOL);	
		player2Start = Point(9, 2, 0, 0, Players::PLAYER2_SYMBOL);
	}
	else if (currentScreen.isSecondScreen())
	{
		player1Start = Point(54, 9, 0, 0, Players::PLAYER1_SYMBOL);
		player2Start = Point(26, 9, 0, 0, Players::PLAYER2_SYMBOL);
	}
	else if (currentScreen.isThirdScreen())
	{
		player1Start = Point(36, 2, 0, 0, Players::PLAYER1_SYMBOL);
		player2Start = Point(43, 2, 0, 0, Players::PLAYER2_SYMBOL);
	}
	
	player1.reset(player1Start);
	player2.reset(player2Start);
	bomb = Bomb();
	player1ReadyForNextScreen = false;
	player2ReadyForNextScreen = false;
	autoBombs.clear();
	
	// Reset M-trap timer on level restart
	mTrapTimerStart = std::chrono::steady_clock::now();
	mTrapVisible = true;
	accumulatedPauseMs = 0;
	accumulatedPauseSec = 0;
}

	

// ==========================================
// Update & Render
// ==========================================

void Game::updateLogic()
{
	// ========================================
	// M-Trap Timer Update (FIRST, before any movement)
	// ========================================
	bool wasVisible = mTrapVisible;
	updateMTrapTimer();
	
	// Death check: M toggled from hidden to visible while player standing on M
	if (!wasVisible && mTrapVisible)
	{
		if (checkMTrapDeath(player1) || checkMTrapDeath(player2))
		{
			resetCurrentGame();
			return;
		}
	}
	
	// ========================================
	// Spring Logic (process before normal movement)
	// ========================================
	
	// Handle compression state (check release conditions)
	updateSpringLogic(player1);
	updateSpringLogic(player2);
	
	// Handle launching state (forced movement with collisions)
	processForcedMove(player1, player2);
	processForcedMove(player2, player1);
	
	// ========================================
	// Normal Movement (only for players NOT in spring mode)
	// ========================================
	if (player1.getSpringState().mode == SpringMode::None)
		updatePlayerMovement(player1);
	if (player2.getSpringState().mode == SpringMode::None)
		updatePlayerMovement(player2);

	// M-trap death check: Player walked onto visible M-trap
	if (checkMTrapDeath(player1) || checkMTrapDeath(player2))
	{
		resetCurrentGame();
		return;
	}

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
	
	if (bomb.isActive())
	{
		if (bomb.tick())
		{
			explodeBomb();
		}
	}
	
	// ========================================
	// Room 3 Boss Update
	// ========================================
	if (currentScreen.isThirdScreen() && room3Boss.isActive())
	{
		// Check if player stepped on a 'B' bomb tile during TaskRunning
		if (room3Boss.getState() == BossState::TaskRunning)
		{
			Point p1Pos = player1.getPosition();
			Point p2Pos = player2.getPosition();
			
			// Check if either player is on a bomb tile
			if (currentScreen.getCharAtPublic(p1Pos) == AUTO_BOMB ||
				currentScreen.getCharAtPublic(p2Pos) == AUTO_BOMB)
			{
				room3Boss.onBombStepped(currentScreen, player1, player2);
				
				// Apply penalties - record each life lost
				int penalty = room3Boss.getLifePenalty();
				for (int i = 0; i < penalty; ++i) {
					lives--;
					if (lives <= 0) {
						lives = 0;
						showGameOverScreen();
						recordGameEnded(score);
						gameOver = true;
						return;
					}
					recordLostLife();
				}
				if (room3Boss.getScorePenalty() > 0) {
					score = (score >= room3Boss.getScorePenalty()) 
						? score - room3Boss.getScorePenalty() : 0;
				}
				room3Boss.clearPenalties();
				return;
			}
		}
		
		// Regular boss update
		room3Boss.update(currentScreen, player1, player2, shouldShowOverlay(), gameTimeMs);
		
		// Apply any penalties generated during update (e.g., timeout)
		int penalty = room3Boss.getLifePenalty();
		for (int i = 0; i < penalty; ++i) {
			lives--;
			if (lives <= 0) {
				lives = 0;
				showGameOverScreen();
				recordGameEnded(score);
				gameOver = true;
				return;
			}
			recordLostLife();
		}
		if (room3Boss.getScorePenalty() > 0) {
			score = (score >= room3Boss.getScorePenalty()) 
				? score - room3Boss.getScorePenalty() : 0;
		}
		room3Boss.clearPenalties();
	}
	
	tryAdvanceToNextScreen();
}

void Game::render()
{
	// Use torch-aware drawing for dark screen support
	currentScreen.drawCurrentWithTorch(player1, player2, mTrapVisible);
	if (currentScreen.getCurrentScreen() != Screens::ScreenId::Final)
	{
		player1.draw();
		player2.draw();
		
		// ========================================
		// Room 3 Boss Rendering
		// ========================================
		if (currentScreen.isThirdScreen() && room3Boss.isActive())
		{
			// Draw boss overlays (countdown)
			room3Boss.drawOverlay();
			
			// Draw task bar instead of normal status bar during task
			if (room3Boss.getState() == BossState::TaskRunning || 
				room3Boss.getState() == BossState::Victory)
			{
				room3Boss.drawTaskBar(currentScreen.getLegendY());
				
				// Also show lives/score on a separate line
				int legendStart = currentScreen.getLegendY();
				gotoxy(0, legendStart + 2);
				std::cout << "Lives:" << lives << "  Score:" << score << "          ";
			}
			else
			{
				drawStatusBar();
			}
			
			// Boss bomb blinking (last 10 seconds) - hide all B tiles when not visible
			if (room3Boss.shouldBombsBlink() && !room3Boss.areBombsCurrentlyVisible())
			{
				// Hide all 'B' tiles by overwriting with space
				for (int y = 0; y < Screen::MAX_Y; ++y) {
					for (int x = 0; x < Screen::MAX_X; ++x) {
						if (currentScreen.getCharAtPublic(Point(x, y)) == AUTO_BOMB) {
							gotoxy(x, y);
							std::cout << ' ';
						}
					}
				}
			}
		}
		else
		{
			drawStatusBar();
		}

		// Bomb blinking animation (using Bomb class)
		if (bomb.shouldBlinkOff()) {
			gotoxy(bomb.getPosition().getX(), bomb.getPosition().getY());
			std::cout << ' ';
		}

		// AutoBomb blinking animation (using AutoBomb class)
		for (const auto& ab : autoBombs) {
			if (ab.shouldBlinkOff()) {
				gotoxy(ab.getPosition().getX(), ab.getPosition().getY());
				std::cout << ' ';
			}
		}

		const Point& p1Pos = player1.getPosition();
		const Point& p2Pos = player2.getPosition();

		// Skip hint display during boss task running (would overwrite task bar)
		bool bossTaskActive = currentScreen.isThirdScreen() && room3Boss.isActive() &&
			(room3Boss.getState() == BossState::TaskRunning || room3Boss.getState() == BossState::Victory);
		
		if (!bossTaskActive)
		{
			// Screen 3: Only show puzzle hint at specific position (8, 11)
			if (currentScreen.isThirdScreen())
			{
				const Point puzzleHintPos(8, 11);
				if (p1Pos == puzzleHintPos || p2Pos == puzzleHintPos)
				{
					gotoxy(0, 23);
					std::cout << "Hint: Find a way to activate the switch";
					gotoxy(0, 24);
					std::cout << "so both players can pass M doors";
				}
				else
				{
					// Clear hint area
					gotoxy(0, 23);
					std::cout << "                                                          ";
					gotoxy(0, 24);
					std::cout << "                                                          ";
				}
			}
			else
			{
				// Screen 1 and 2: Use normal hint logic
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


	}
}

void Game::drawStatusBar()
{
	// Get legend Y position from current screen (where 'L' was found in file)
	int legendStart = currentScreen.getLegendY();
	
	// Line 1: Player items, lives, score, color status
	gotoxy(0, legendStart);
	std::cout << "P1:[";
	char item1 = player1.getHeldItem();
	if (g_colorsEnabled && item1 != ' ')
		setConsoleColor(getColorForChar(item1));
	std::cout << item1;
	if (g_colorsEnabled)
		resetColor();
	std::cout << "]  P2:[";
	char item2 = player2.getHeldItem();
	if (g_colorsEnabled && item2 != ' ')
		setConsoleColor(getColorForChar(item2));
	std::cout << item2;
	if (g_colorsEnabled)
		resetColor();
	std::cout << "]  ";
	std::cout << "Lives:" << lives << "  Score:" << score;
	std::cout << "  [" << (g_colorsEnabled ? "COLOR" : "MONO") << "]          ";

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

	if (currentScreen.isObstacle(nextPos))
	{
		// Compute player's push force based on spring state
		Direction moveDir = getPlayerInputDirection(player);
		int force = player.computePushForce(moveDir);
		
		// Try to push with computed force
		bool Pushable = currentScreen.tryPushObstacle(nextPos, moveDir, force, getOtherPlayer(player));
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
		
	if (currentScreen.isDoor(nextPos) && Key::playerHasKey(player))
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
	// ========================================
	// Spring Entry Detection
	// ========================================
	else if (currentScreen.isSpring(nextPos))
	{
		Spring* spring = currentScreen.getSpringAt(nextPos);
		if (spring)
		{
			// Check if player is approaching from correct direction
			Direction moveDir = getPlayerInputDirection(player);
			if (spring->canCompress(moveDir))
			{
				// Enter compression mode
				player.handleSpringEntry(spring->getId());
				player.incrementCompression();  // First step counts as compression
				player.move();
				return;
			}
		}
		// If can't compress, treat as normal walkable cell
		player.move();
	}
	else if (currentScreen.isRiddle(nextPos))
	{
		if (handleRiddleEncounter(player, nextPos))
			return;
	}
	// Block movement onto M tiles based on type:
	// - Size 1 M: always blocks (always visible)
	// - Size > 1 M: when visible, player moves onto it and dies (handled separately)
	//               when hidden (mTrapVisible=false), player can walk through
	else if (currentScreen.isMTrap(nextPos))
	{
		if (!currentScreen.isMTrapDeadly(nextPos))
		{
			// Size-1 M always blocks
			player.stop();
			return;
		}
		// Size > 1 M - check if visible
		if (mTrapVisible)
		{
			// Player walks onto deadly visible M - will die after movement
			player.move();
		}
		else
		{
			// Deadly M is hidden - player can walk through
			player.move();
		}
	}

	else if (currentScreen.isFreeCellForPlayer(nextPos))
	{
		player.move();
	}
	else
	{
		player.stop();
	}
	
	// M-trap death check: if player moved onto a visible M-trap, they die
	// Note: This is called from updateLogic() which handles the reset
}

void Game::collectItemIfPossible(Player& player)
{
	const Point& pos = player.getPosition();

	// Can only hold one item at a time
	if (Key::playerHasKey(player) || player.hasBomb() || Torch::playerHasTorch(player))
		return;

	if (currentScreen.isKey(pos))
	{
		Key::onPickup(player);
		currentScreen.makePassage(pos);
	}
	else if (currentScreen.isBomb(pos))
	{
		if (bomb.isActive() &&
			pos.getX() == bomb.getPosition().getX() &&
			pos.getY() == bomb.getPosition().getY())
		{
			return;
		}

		player.collectBomb();
		currentScreen.makePassage(pos);
	}
	else if (currentScreen.isTorch(pos))
	{
		Torch::onPickup(player);
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
	// Use Bomb class to handle placement
	bomb.place(player, currentScreen);
}

void Game::explodeBomb()
{
	if (!bomb.isActive())
		return;

	Point center = bomb.getPosition();
	int R2 = bomb.getRadiusSquared();
	
	// Execute explosion using Bomb class
	bomb.explode(currentScreen);

	// Check player deaths
	bool p1Dead = isPlayerInExplosion(player1, center, R2);
	bool p2Dead = isPlayerInExplosion(player2, center, R2);

	if (p1Dead || p2Dead)
	{
		decrementLife();
		if (!gameOver)
		{
			currentScreen.resetCurrent();
			if (currentScreen.isFirstScreen())
			{
				player1Start = Point(5, 2, 0, 0, Players::PLAYER1_SYMBOL);
				player2Start = Point(9, 2, 0, 0, Players::PLAYER2_SYMBOL);
			}
			else if (currentScreen.isSecondScreen())
			{
				player1Start = Point(54, 9, 0, 0, Players::PLAYER1_SYMBOL);
				player2Start = Point(26, 9, 0, 0, Players::PLAYER2_SYMBOL);
			}
			player1.reset(player1Start);
			player2.reset(player2Start);
			bomb = Bomb();
			player1ReadyForNextScreen = false;
			player2ReadyForNextScreen = false;
			autoBombs.clear();
		}
	}
}

bool Game::handleAutoBombs()
{
	std::vector<Point> centers;
	currentScreen.collectPendingAutoBombs(centers);

	// Create AutoBomb objects for new triggers
	for (const Point& c : centers)
	{
		autoBombs.push_back(AutoBomb(c));
	}

	if (autoBombs.empty())
		return false;

	bool someoneDied = false;

	for (size_t i = 0; i < autoBombs.size(); )
	{
		AutoBomb& ab = autoBombs[i];

		// Use AutoBomb tick method
		if (!ab.tick())
		{
			++i;
			continue;
		}

		// Explode using AutoBomb class
		Point center = ab.getPosition();
		int R2 = ab.getRadiusSquared();
		ab.explode(currentScreen);

		bool p1Dead = isPlayerInExplosion(player1, center, R2);
		bool p2Dead = isPlayerInExplosion(player2, center, R2);

		if (p1Dead || p2Dead)
			someoneDied = true;

		autoBombs.erase(autoBombs.begin() + i);
	}
	return someoneDied;
}

bool Game::isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared) const
{
	Point p = player.getPosition();
	int dx = p.getX() - center.getX();
	int dy = p.getY() - center.getY();

	return (dx * dx + dy * dy <= radiusSquared);
}

void Game::dropTorch(Player& player)
{
	// Use Torch class to handle drop
	Torch::onDrop(player, currentScreen);
}

bool Game::handleRiddleEncounter(Player& player, const Point& nextPos)
{
	Riddle* r = currentScreen.getRiddleAt(nextPos);
	if (!r)
	{
		player.stop();
		return true;
	}

	// Polymorphic solve riddle (interactive or file-based)
	bool ok = solveRiddle(r);

	if (ok)
	{
		currentScreen.removeRiddleAt(nextPos);
		player.move();
	}
	else
	{
		score = (score >= 400 ? score - 400 : 0);
		player.stop();
	}

	drawStatusBar();
	return true;
}

bool Game::solveRiddle(Riddle* r)
{
	// Base implementation: Interactive riddle solving (Screen display + Keyboard input)
	
	const int y = currentScreen.getLegendY();
	int x = 0;
	auto clearLine = [&](int yy) {
		gotoxy(0, yy);
		std::cout << "                                                                                ";
	};

	// Clear area
	for (int i = 0; i <= 3; ++i) clearLine(y + i);

	// Display Riddle
	const auto& opts = r->getOptions();

	gotoxy(x, y);
	std::cout << "RIDDLE: " << r->getQuestion();

	gotoxy(x, y + 1);
	std::cout << "A) " << opts[0] << "   B) " << opts[1];

	gotoxy(x, y + 2);
	std::cout << "C) " << opts[2] << "   D) " << opts[3];

	gotoxy(x, y + 3);
	std::cout << "Choose (A-D / 1-4): ";

	// Get Input
	char ch = _getch(); // Interactive input via console (base Game behavior)
	std::string input(1, ch);
	int choiceIndex = Riddle::parseChoice(input);

	bool ok = false;
	if (choiceIndex != -1)
		ok = r->trySolve(choiceIndex);

	// Record riddle result (virtual call - saves in GameKeyboard, does nothing in Game)
	recordRiddle(r->getQuestion(), input, ok);

	// Display Result
	for (int i = 0; i <= 3; ++i) clearLine(y + i);
	gotoxy(0, y + 1);

	if (ok)
	{
		std::cout << "Correct! Press any key to continue...";
	}
	else
	{
		std::cout << "Wrong! -400 score. Press any key to continue...";
	}

	_getch(); // Wait for key press

	// Cleanup
	for (int i = 0; i <= 3; ++i) clearLine(y + i);

	return ok;
}



// ==========================================
// Spring Logic
// ==========================================

Direction Game::getPlayerInputDirection(const Player& player) const
{
	const Point& pos = player.getPosition();
	int dx = pos.getDiffX();
	int dy = pos.getDiffY();
	
	if (dy < 0) return Direction::UP;
	if (dy > 0) return Direction::DOWN;
	if (dx < 0) return Direction::LEFT;
	if (dx > 0) return Direction::RIGHT;
	return Direction::STAY;
}

bool Game::isBlockedForFlight(const Point& pos) const
{
	// Check if position is blocked for spring flight
	if (pos.getX() < 0 || pos.getX() >= Screens::MAX_X ||
		pos.getY() < 0 || pos.getY() >= Screens::MAX_Y)
		return true;
	
	// Note: Obstacles handled separately in processForcedMove (may be pushable)
	return currentScreen.isWall(pos) || 
	       currentScreen.isDoor(pos) ||
	       currentScreen.isunbreakable_wall(pos);
}

bool Game::isPerpendicular(Direction d1, Direction d2) const
{
	// Check if two directions are perpendicular
	bool d1Vertical = (d1 == Direction::UP || d1 == Direction::DOWN);
	bool d2Vertical = (d2 == Direction::UP || d2 == Direction::DOWN);
	
	// One vertical and one horizontal = perpendicular
	return d1Vertical != d2Vertical;
}

void Game::updateSpringLogic(Player& player)
{
	Player::SpringState& state = player.getSpringState();
	if (state.mode != SpringMode::Compressing)
		return;
	
	// Find the spring the player is on
	Spring* spring = currentScreen.getSpringAt(player.getPosition());
	if (!spring) {
		player.resetSpringState();
		return;
	}
	
	// Get player's current input direction
	Direction inputDir = getPlayerInputDirection(player);
	
	// Check release conditions
	bool shouldRelease = false;
	
	// Condition 1: Player stopped (not pushing anymore)
	if (inputDir == Direction::STAY)
		shouldRelease = true;
	
	// Condition 2: Direction changed (no longer pushing toward spring)
	if (inputDir != Direction::STAY && inputDir != spring->getPushDirection())
		shouldRelease = true;
	
	// Condition 3: Next position is blocked or outside spring
	Point nextPos = player.getPosition();
	nextPos.move();
	
	if (!currentScreen.isSpring(nextPos) || isBlockedForFlight(nextPos))
		shouldRelease = true;
	
	if (shouldRelease)
	{
		// Calculate launch parameters based on compression + inherited momentum
		// Minimum 1 compression to launch
		if (state.compressedCount > 0)
		{
			int totalCompression = state.compressedCount + state.inheritedMomentum;
			SpringLaunchParams params = spring->calculateLaunchParams(totalCompression);
			player.launch(params, spring->getReleaseDirection());
		}
		else
		{
			// No compression - just exit spring mode
			player.resetSpringState();
		}
	}
	else
	{
		// Continue compressing: increment count and move deeper
		player.incrementCompression();
		player.move();
	}
}

void Game::processForcedMove(Player& player, Player& otherPlayer)
{
	Player::SpringState& state = player.getSpringState();
	if (state.mode != SpringMode::Launching)
		return;
	
	// Calculate movement deltas for launch direction
	int dx = 0, dy = 0;
	switch (state.launchDir)
	{
		case Direction::UP:    dy = -1; break;
		case Direction::DOWN:  dy =  1; break;
		case Direction::LEFT:  dx = -1; break;
		case Direction::RIGHT: dx =  1; break;
		default: break;
	}
	
	// Process 'speed' sub-steps of forced movement
	for (int i = 0; i < state.launchSpeed; ++i)
	{
		Point currentPos = player.getPosition();
		Point nextPos(currentPos.getX() + dx, currentPos.getY() + dy);
		
		// Check for pushable obstacle (spring-boosted push)
		if (currentScreen.isObstacle(nextPos))
		{
			// Compute spring-boosted force
			int force = player.computePushForce(state.launchDir);
			
			// Attempt push with spring force
			bool pushed = currentScreen.tryPushObstacle(nextPos, state.launchDir, force, otherPlayer);
			
			if (pushed)
			{
				// Obstacle moved - player continues
				currentScreen.makePassage(nextPos);
				player.setPosition(nextPos);
				continue;
			}
			else
			{
				// Obstacle too heavy - cancel spring immediately
				player.resetSpringState();
				return;
			}
		}
		
		// Check for spring entry during flight (spring chaining)
		if (currentScreen.isSpring(nextPos))
		{
			Spring* spring = currentScreen.getSpringAt(nextPos);
			if (spring && spring->canCompress(state.launchDir))
			{
				// Enter chain compression mode - inherit current momentum
				player.handleSpringEntry(spring->getId(), state.launchSpeed);
				player.setPosition(nextPos);
				player.incrementCompression();  // First cell counts as compression
				return;  // Exit forced move - spring logic takes over
			}
			// If can't compress (wrong direction), just pass through
		}
		
		// Check collision with wall/door
		if (isBlockedForFlight(nextPos))
		{
			player.resetSpringState();
			return;
		}
		
		// Check collision with other player
		if (nextPos == otherPlayer.getPosition())
		{
			// Transfer momentum to other player
			otherPlayer.absorbMomentum(state);
			
			// Sender continues walking (not launching) in same direction
			// This enables cooperative pushing after momentum transfer
			player.resetSpringState();
			player.setDirection(state.launchDir);
			player.move();
			return;
		}
		
		// Clear path - move player
		player.setPosition(nextPos);
		
		// Update switches at this position (fixes spring-launched switch activation)
		currentScreen.updateSwitchStates(player, otherPlayer);
	}
	
	// Lateral Movement: Allow ONE perpendicular move per frame
	Direction inputDir = getPlayerInputDirection(player);
	if (inputDir != Direction::STAY && isPerpendicular(inputDir, state.launchDir))
	{
		int latDx = 0, latDy = 0;
		switch (inputDir)
		{
			case Direction::UP:    latDy = -1; break;
			case Direction::DOWN:  latDy =  1; break;
			case Direction::LEFT:  latDx = -1; break;
			case Direction::RIGHT: latDx =  1; break;
			default: break;
		}
		
		Point lateralPos(player.getPosition().getX() + latDx,
						 player.getPosition().getY() + latDy);
		
		// Check if lateral position has pushable obstacle
		if (currentScreen.isObstacle(lateralPos))
		{
			// Lateral movement uses force=1 (not spring boosted)
			int lateralForce = player.computePushForce(inputDir);
			bool pushed = currentScreen.tryPushObstacle(lateralPos, inputDir, lateralForce, otherPlayer);
			
			if (pushed)
			{
				currentScreen.makePassage(lateralPos);
				player.setPosition(lateralPos);
			}
			// If push fails, just skip lateral move (flight continues)
		}
		else if (!isBlockedForFlight(lateralPos) && 
				 lateralPos != otherPlayer.getPosition())
		{
			player.setPosition(lateralPos);
		}
	}
	
	// Decrement flight timer
	if (player.tickFlight())
	{
		// Flight finished (ticksLeft reached 0)
		// tickFlight already resets state when done
	}
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

		// Record screen change for save/load verification
		recordScreenChange(static_cast<int>(exit.to));

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
			recordGameEnded(score);  // Record victory with final score
			_getch();      
			gameOver = true;
		}
		else
		{
			// Reset timer for next level
			levelStartTime = getCurrentTimeSeconds();
			
			// Show STORY_2 when entering Screen 2 for first time (not in load mode)
			if (currentScreen.isSecondScreen() && !shownStory2 && shouldShowOverlay()) {
				render();
				showStoryOverlay(2);
				shownStory2 = true;
			}
			
			// Initialize Room 3 Boss if transitioning to Room 3
			if (currentScreen.isThirdScreen()) {
				room3Boss.init();
			}
			
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
	recordLostLife();  // Record for save/load verification
	if (lives <= 0)
	{
		recordGameEnded(score);  // Record game ending
		showGameOverScreen();
		gameOver = true;
	}
}

void Game::addLevelCompletionScore()
{
	// Time-based score (faster = more points)
	// Subtract accumulated pause time to freeze timer during pause
	int elapsedSeconds = getCurrentTimeSeconds() - levelStartTime;
	if (elapsedSeconds < 0) elapsedSeconds = 0;  // Safety check
	int timeScore;
	if (elapsedSeconds <= Score::TIER1_SECONDS)        // Under 1 minute
		timeScore = Score::TIER1_POINTS;
	else if (elapsedSeconds <= Score::TIER2_SECONDS)  // 1-2 minutes
		timeScore = Score::TIER2_POINTS;
	else if (elapsedSeconds <= Score::TIER3_SECONDS)  // 2-3 minutes
		timeScore = Score::TIER3_POINTS;
	else if (elapsedSeconds <= Score::TIER4_SECONDS)  // 3-5 minutes
		timeScore = Score::TIER4_POINTS;
	else                             // Over 5 minutes
		timeScore = Score::TIER5_POINTS;

	// Lives bonus (remaining lives x multiplier)
	int livesBonus = lives * Score::LIVES_BONUS_MULTIPLIER;

	score += timeScore + livesBonus;
}

void Game::showGameOverScreen()
{
	// In silent mode (load -silent), skip the visual Game Over screen
	// to allow immediate reporting of verification results
	if (getSleepDuration() == 0) {
		return;
	}

	cls();
	// Use FINAL_SCREEN_TEMPLATE (which shows "GAME OVER")
	currentScreen.setCurrentScreen(Screens::ScreenId::Final);
	currentScreen.drawCurrent();
	
	gotoxy(30, 17);
	std::cout << "Final Score: " << score;
	gotoxy(22, 19);
	std::cout << "Press any key to return to the main menu...";
	
    // Wait for key
    if (hasInput()) getNextInput(); // Flush one key
	_getch();
}

int Game::getCurrentTimeSeconds() const
{
	// Return deterministic game time in seconds
	return static_cast<int>(gameTimeMs / 1000);
}

// ==========================================
// M-Trap Logic
// ==========================================

void Game::updateMTrapTimer()
{
	// Use deterministic gameTimeMs
	long long elapsed = gameTimeMs;

	// Calculate position in cycle (0 to CYCLE_MS-1)
	int cyclePos = static_cast<int>(elapsed % Timing::M_TRAP_CYCLE_MS);
	
	// Visible during first half of cycle
	mTrapVisible = (cyclePos < Timing::M_TRAP_VISIBLE_MS);
}

bool Game::isMTrapVisible() const
{
	return mTrapVisible;
}

bool Game::checkMTrapDeath(const Player& player) const
{
	if (!mTrapVisible)
		return false;
	
	const Point& pos = player.getPosition();
	char cell = currentScreen.getCharAtPublic(pos);
	
	// Only deadly M (size > 1) kills player
	// Size-1 M just blocks movement, doesn't kill
	if (cell != Tiles::M_TRAP)
		return false;
	
	return currentScreen.isMTrapDeadly(pos);
}

// ==========================================
// Story Overlay System
// ==========================================

void Game::showStoryOverlay(int storyNumber)
{
	cls();
	
	if (storyNumber == 1)
	{
		// STORY_1: Game start - Goni's Warning #1
		printCentered("=== GONI'S WARNING #1 ===", 5);
		printCentered("Welcome, brave geniuses. I'm Goni.", 8);
		printCentered("This maze is not a game - it's a career-ending decision.", 10);
		printCentered("Fun fact: 73 people tried to beat it. 0 succeeded.", 12);
		printCentered("If you value your dignity, turn back now.", 14);
		printCentered("(Yes, I'm counting you as 74 and 75. Don't argue.)", 16);
	}
	else if (storyNumber == 2)
	{
		// STORY_2: Screen 2 entry - Goni's Warning #2
		printCentered("=== GONI'S WARNING #2 ===", 5);
		printCentered("You made it here?!", 8);
		printCentered("Wow. This brings back memories...", 10);
		printCentered("The last time I felt this shocked was during my 12th divorce.", 12);
		printCentered("Same confusion. Same regret.", 14);
		printCentered("Turn back now. Let's not make this number 13.", 16);
	}
	
	printCentered("Press any key to continue...", 20);
	_getch();
	
	// Clear and re-render the game
	cls();
	render();
}

// ==========================================
// Virtual Functions - Base Implementation
// (Override in derived classes for different modes)
// ==========================================

bool Game::hasInput()
{
	// Base implementation: check keyboard
	return _kbhit() != 0;
}

char Game::getNextInput()
{
	// Base implementation: read from keyboard
	return _getch();
}

void Game::doRender()
{
	// Base implementation: full render
	render();
}

int Game::getSleepDuration() const
{
	// Base implementation: normal game speed
	return Timing::GAME_TICK_MS;
}

bool Game::shouldShowOverlay() const
{
	// Base implementation: show overlays
	return true;
}

void Game::recordStep(char p1Key, char p2Key)
{
	// Base implementation: do nothing (not in save mode)
}

void Game::recordLostLife()
{
	// Base implementation: do nothing
}

void Game::recordScreenChange(int screenNumber)
{
	// Base implementation: do nothing
}

void Game::recordRiddle(const std::string& question, const std::string& answer, bool correct)
{
	// Base implementation: do nothing
}

void Game::recordGameEnded(int finalScore)
{
	// Base implementation: do nothing
}

void Game::recordBossTaskComplete(int taskNumber)
{
	// Base implementation: do nothing
}

bool Game::shouldFinishGame() const
{
	// Base implementation: never force finish (wait for user exit/game over)
	return false;
}
