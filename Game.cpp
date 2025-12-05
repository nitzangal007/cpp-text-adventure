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

	if (currentScreen.isObstacle(nextPos))
	{
		bool Pushable = obstaclePushable(nextPos, player);
		if (Pushable)
		{
			Point obstacleNewPos = nextPos;
			obstacleNewPos.move();
			currentScreen.setCharAt(obstacleNewPos, Screens::OBSTACLE);
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


Player& Game::getOtherPlayer(const Player& p)
{
	if (p.getId() == Player::Id::First)
		return player2;
	else
		return player1;
}


bool Game::obstaclePushable(const Point& nextPos, Player& player)				//check if obstacle(s) can be pushed
{
	int dx = nextPos.getX() - player.getPosition().getX();
	int dy = nextPos.getY() - player.getPosition().getY();
	Point currPos = player.getPosition();
	Point obstacleCell = nextPos;

	if (std::abs(dx) + std::abs(dy) != 1)								// we need to push in straight line		
		return false;


	std::vector<Point> group;
	collectObstacleGroup(nextPos, group);

	int obstacleSize = (int)group.size();

	int availableForce = 1; 

	Player& other = getOtherPlayer(player);

	Point p1 = player.getPosition();
	Point p2 = other.getPosition();

	int dist = std::abs(p1.getX() - p2.getX()) +
		std::abs(p1.getY() - p2.getY());
	bool adjacent = (dist == 1);

	Point next1 = p1; next1.move();
	Point next2 = p2; next2.move();

	int dx1 = next1.getX() - p1.getX();
	int dy1 = next1.getY() - p1.getY();
	int dx2 = next2.getX() - p2.getX();
	int dy2 = next2.getY() - p2.getY();

	bool sameDir = (dx1 == dx2 && dy1 == dy2);

	if (adjacent && sameDir)
		availableForce += 1;          

	if (availableForce < obstacleSize)
		return false; 

	for (size_t i = 0; i < group.size(); ++i)
	{
		Point cell = group[i];
		Point target(cell.getX() + dx, cell.getY() + dy, 0, 0, ' ');

		if (!currentScreen.isInside(target))
			return false;
		if (currentScreen.isWall(target) || currentScreen.isDoor(target))
			return false;

		if (currentScreen.isObstacle(target))
			return false;
		
			

		int obstacleSize = (int)group.size();



		moveObstacleGroup(group, dx, dy);




	}
}
void Game::collectObstacleGroup(const Point& start, std::vector<Point>& group) const	// collect all connected obstacles
{
	const Direction dirs[4] = {
	Direction::UP,
	Direction::RIGHT,
	Direction::DOWN,
	Direction::LEFT
	};

	group.clear();      // make sure the group is empty

	if (!currentScreen.isObstacle(start))								// starting point is not an obstacle
		return;

	group.push_back(start);
	for (int i = 0; i < group.size(); i++)
	{
		Point current = group[i];
		for (int d = 0; d < 4; ++d)
		{
			int dx = 0, dy = 0;
			switch (dirs[d])
			{
			case Direction::UP:
				dx = 0;  dy = -1;
				break;
			case Direction::RIGHT:
				dx = 1;  dy = 0;
				break;
			case Direction::DOWN:
				dx = 0;  dy = 1;
				break;
			case Direction::LEFT:
				dx = -1; dy = 0;
				break;
			case Direction::STAY:
				dx = 0;  dy = 0;
				break;
			}
			int nx = current.getX() + dx;
			int ny = current.getY() + dy;

			Point neighbour(nx, ny, 0, 0, ' ');

			if (!currentScreen.isInside(neighbour))
				continue;

			if (!currentScreen.isObstacle(neighbour))
				continue;

			bool alreadyInGroup = false;
			for (size_t j = 0; j < group.size(); ++j)
			{
				if (group[j].getX() == neighbour.getX() &&
					group[j].getY() == neighbour.getY())
				{
					alreadyInGroup = true;
					break;
				}
			}
			if (!alreadyInGroup)
			{
				group.push_back(neighbour);
			}

		}

	}


}

void Game::moveObstacleGroup(const std::vector<Point>& group, int dx, int dy)		// move all obstacles in the group 
{
	
	for (size_t i = 0; i < group.size(); ++i)								//remove * from old positions
		currentScreen.setCharAt(group[i], Screens::EMPTY_SPACE);

	
	for (size_t i = 0; i < group.size(); ++i)								//set * in new positions
	{
		Point cell = group[i];
		Point target(cell.getX() + dx, cell.getY() + dy, 0, 0, ' ');
		currentScreen.setCharAt(target, Screens::OBSTACLE);
	}
}



void Game::initGame() {
	// Initialize game state, load resources, etc.
	cls();
	currentScreen.init();
	currentScreen.initFirstScreenSwitches();
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

	if (currentScreen.isFirstScreen())
		currentScreen.updateSwitchStates(player1, player2);
	

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





