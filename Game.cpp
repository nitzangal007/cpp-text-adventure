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


	Player& other = getOtherPlayer(player);
	Point otherPos = other.getPosition();
	if (nextPos.getX() == otherPos.getX() &&
		nextPos.getY() == otherPos.getY() && !isExitWaitPosition(nextPos))
	{
		player.stop();
		return;
	}

	
		


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

	// If player already holds any item – do NOT pick up a new one
	if (player.hasKey() || player.hasBomb() || player.hasTorch())
		return;

	if (currentScreen.isKey(pos))
	{
		player.collectKey();
		currentScreen.makePassage(pos);
	}
	else if (currentScreen.isBomb(pos))
	{
		// Do not "collect" an already placed active bomb on the board
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

void Game::drawStatusBar()
{
	gotoxy(0, Screens::MAX_Y-4); 


	std::cout << "Player 1 holding: [" << player1.getHeldItem() << "]";

	gotoxy(0, Screens::MAX_Y-1); 
	std::cout << "Player 2 holding: [" << player2.getHeldItem() << "]";
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

	const int RADIUS = 2;
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

bool Game::isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared)
{
	Point p = player.getPosition();
	int dx = p.getX() - center.getX();
	int dy = p.getY() - center.getY();

	return (dx * dx + dy * dy <= radiusSquared);
}

void Game::resetCurrentGame()
{
	currentScreen.resetCurrent();
	player1.reset(player1Start);
	player2.reset(player2Start);
	bomb.active = false;
	bomb.ticksLeft = 0;
	player1ReadyForNextScreen = false;
	player2ReadyForNextScreen = false;
	autoBombs.clear();
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

		player1Start = exit.nextStartP1;
		player2Start = exit.nextStartP2;

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

bool Game::handleAutoBombs()
{
	// 1. פצצות חדשות שמופעלות בפריים הזה ע"י סוויצ'ים
	std::vector<Point> centers;
	currentScreen.collectPendingAutoBombs(centers);

	constexpr int AUTO_BOMB_DELAY = 19;

	for (const Point& c : centers)
	{
		AutoBomb ab;
		ab.center = c;
		ab.ticksLeft = AUTO_BOMB_DELAY;
		autoBombs.push_back(ab);
	}

	// אם אין בכלל פצצות אוטומטיות פעילות – אין מה לעשות
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
			// עוד לא הגיע זמן הפיצוץ – רק להוריד טיק
			--ab.ticksLeft;
			++i;
			continue;
		}

		currentScreen.clearExplosionArea(ab.center, RADIUS);

		// זמן פיצוץ – בודקים אם שחקנים ברדיוס
		bool p1Dead = isPlayerInExplosion(player1, ab.center, R2);
		bool p2Dead = isPlayerInExplosion(player2, ab.center, R2);

		if (p1Dead || p2Dead)
			someoneDied = true;
		
		// עכשיו מוחקים את ה-B מהמפה
		currentScreen.setCharAt(ab.center, Screens::EMPTY_SPACE);

		// ומוחקים את הפצצה מהרשימה
		autoBombs.erase(autoBombs.begin() + i);
		// לא מגדילים i כי מחקנו את האיבר הנוכחי
	}
	return someoneDied;
}




bool Game::obstaclePushable(const Point& nextPos, Player& player)   // check if obstacle(s) can be pushed
{
	
	Point currPos = player.getPosition();
	int dx = nextPos.getX() - currPos.getX();
	int dy = nextPos.getY() - currPos.getY();
	Point obstacleCell = nextPos;  

	
	if (std::abs(dx) + std::abs(dy) != 1)
		return false;

	
	std::vector<Point> group;
	collectObstacleGroup(nextPos, group);

	if (group.empty())
		return false; 

	int obstacleSize = (int)group.size();

	

	int availableForce = 1;         

	Player& other = getOtherPlayer(player);
	Point otherPos = other.getPosition();
	Point otherNext = otherPos;
	otherNext.move();              

	int odx = otherNext.getX() - otherPos.getX();
	int ody = otherNext.getY() - otherPos.getY();


	bool sameDir = (odx == dx && ody == dy);

	if (sameDir)
	{
		
		bool otherBehind =
			(otherPos.getX() == currPos.getX() - dx) &&
			(otherPos.getY() == currPos.getY() - dy);

		if (otherBehind)
		{
			++availableForce;
		}
		else
		{
		
			int dist = std::abs(otherPos.getX() - currPos.getX()) +
				std::abs(otherPos.getY() - currPos.getY());
			bool adjacent = (dist == 1);  

			if (adjacent)
			{
				
				if (currentScreen.isObstacle(otherNext))
				{
					
					bool inSameGroup = false;
					for (size_t i = 0; i < group.size(); ++i)
					{
						if (group[i].getX() == otherNext.getX() &&
							group[i].getY() == otherNext.getY())
						{
							inSameGroup = true;
							break;
						}
					}

					if (inSameGroup)
					{
						++availableForce;   
					}
				}
			}
		}
	}

	
	if (availableForce < obstacleSize)
		return false;



	for (size_t i = 0; i < group.size(); ++i)
	{
		const Point& cell = group[i];
		Point target(cell.getX() + dx, cell.getY() + dy, 0, 0, ' ');

		
		if (!currentScreen.isInside(target))
			return false;

		if (target.getX() == otherPos.getX() &&
			target.getY() == otherPos.getY())
		{
			return false;
		}

		
		if (currentScreen.isWall(target) || currentScreen.isDoor(target))
			return false;

		
		if (currentScreen.isObstacle(target))
		{
			bool inSameGroup = false;
			for (size_t j = 0; j < group.size(); ++j)
			{
				if (group[j].getX() == target.getX() &&
					group[j].getY() == target.getY())
				{
					inSameGroup = true;
					break;
				}
			}

			if (!inSameGroup)
				return false; 
		}
	}



	moveObstacleGroup(group, dx, dy);

	return true;
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
	currentScreen.initSecondScreenSwitches();
	
	currentScreen.setCurrentScreen(Screens::ScreenId::Second);
	player1Start = Point(54, 9, 0, 0, '$'),   // דוגמה – קואורדינטות במסך 2
	player2Start = Point(6, 18, 0, 0, '&');

	player1.reset(player1Start);
	player2.reset(player2Start);
	/*currentScreen.initFirstScreenSwitches();
	player1.reset(player1Start);
	player2.reset(player2Start);*/
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





