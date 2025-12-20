#include "Screens.h"
#include <cmath>

Screens::Screens() = default;

// ==========================================
// Screen File Loading
// ==========================================

// Discovers adv-world*.screen files in the current working directory.
// Files are sorted lexicographically as required by spec.
// Uses Windows API for compatibility with older C++ standards.
bool Screens::discoverScreenFiles()
{
	screenFilePaths.clear();
	
	// Use Windows API to find files matching pattern
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA("adv-world*.screen", &findData);
	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// No files found - this is not an error, just empty result
		return false;
	}
	
	do
	{
		// Skip directories
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			screenFilePaths.push_back(findData.cFileName);
		}
	} while (FindNextFileA(hFind, &findData));
	
	FindClose(hFind);
	
	// Sort lexicographically so _01 < _02 < _03
	std::sort(screenFilePaths.begin(), screenFilePaths.end());
	
	return !screenFilePaths.empty();
}

// Loads a single screen from a .screen file into boards[screenIndex].
// Detects the 'L' legend anchor and stores its Y position.
bool Screens::loadScreenFromFile(int screenIndex, const std::string& path)
{
	std::ifstream file(path);
	
	if (!file.is_open())
	{
		loadingError = "Cannot open file: " + path;
		loadingFailed = true;
		return false;
	}
	
	// Initialize board with spaces
	for (int y = 0; y < MAX_Y; ++y)
	{
		for (int x = 0; x < MAX_X; ++x)
		{
			boards[screenIndex][y][x] = EMPTY_SPACE;
		}
	}
	
	legendY[screenIndex] = -1;  // Default: no legend anchor found
	
	std::string line;
	int y = 0;
	
	while (std::getline(file, line) && y < MAX_Y)
	{
		for (int x = 0; x < MAX_X && x < static_cast<int>(line.size()); ++x)
		{
			char ch = line[x];
			
			// Check for legend anchor 'L'
			if (ch == LEGEND_ANCHOR)
			{
				legendY[screenIndex] = y;
				boards[screenIndex][y][x] = EMPTY_SPACE;
			}
			else
			{
				boards[screenIndex][y][x] = ch;
			}
		}
		++y;
	}
	
	file.close();
	
	// If no legend anchor found, use default position
	if (legendY[screenIndex] < 0)
	{
		legendY[screenIndex] = MAX_Y - 4;
	}
	
	return true;
}

// Returns the Y coordinate where the legend/status bar should start
int Screens::getLegendY() const
{
	int idx = static_cast<int>(current);
	if (legendY[idx] >= 0)
		return legendY[idx];
	return MAX_Y - 4;
}

// ==========================================
// General Screen Management
// ==========================================

void Screens::init()
{
	loadingFailed = false;
	loadingError.clear();
	pendingAutoBombs.clear();
	
	// Step 1: Discover screen files
	if (!discoverScreenFiles())
	{
		if (!loadingFailed)  // No files found (not a filesystem error)
		{
			loadingError = "No screen files found! Place adv-world*.screen files in the game directory.";
			loadingFailed = true;
		}
		return;
	}
	
	// Step 2: Check we have enough screen files
	if (screenFilePaths.size() < NUM_SCREENS)
	{
		loadingError = "Not enough screen files! Found " + 
					   std::to_string(screenFilePaths.size()) + 
					   ", need " + std::to_string(NUM_SCREENS) + ".";
		loadingFailed = true;
		return;
	}
	
	// Step 3: Load each screen from its file
	for (int i = 0; i < NUM_SCREENS; ++i)
	{
		if (!loadScreenFromFile(i, screenFilePaths[i]))
		{
			return;  // loadingError already set
		}
	}
	
	// Step 4: Initialize screen-specific data (switches, door positions, etc.)
	door7Pos.setPosition(71, 11);  // Set door7 position for screen 2
	initFirstScreenSwitches();
	initSecondScreenSwitches();
	
	current = ScreenId::First;
}

void Screens::resetCurrent()
{
	int idx = static_cast<int>(current);
	
	// Reload current screen from file if files were loaded
	if (idx < static_cast<int>(screenFilePaths.size()))
	{
		loadScreenFromFile(idx, screenFilePaths[idx]);
	}
	
	if (current == ScreenId::First)
	{
		initFirstScreenSwitches();
		leftEntranceClosed = false;
		rightEntranceClosed = false;
	}
	else if (current == ScreenId::Second)
	{
		door7Pos.setPosition(71, 11);
		initSecondScreenSwitches();
	}
}

void Screens::setCurrentScreen(ScreenId id) 
{
	current = id;
}

Screens::ScreenId Screens::getCurrentScreen() const 
{
	return current;
}

void Screens::drawCurrent() const
{
	const int screenIndex = static_cast<int>(current);
	for (int y = 0; y < MAX_Y; ++y)
	{
		gotoxy(0, y);
		std::string line;
		line.reserve(MAX_X);
		for (int x = 0; x < MAX_X; ++x)
		{
			line += boards[screenIndex][y][x];
		}
		std::cout << line;
	}
}

void Screens::drawCurrentWithTorch(const Player& p1, const Player& p2) const
{
	const int screenIndex = static_cast<int>(current);
	bool dark = screenIsDark[screenIndex];
	
	for (int y = 0; y < MAX_Y; ++y)
	{
		gotoxy(0, y);
		std::string line;
		line.reserve(MAX_X);
		for (int x = 0; x < MAX_X; ++x)
		{
			char cell = boards[screenIndex][y][x];
			if (dark && !isIlluminated(x, y, p1, p2) && cell != TORCH)
				line += DARKNESS_CHAR;
			else
				line += cell;
		}
		std::cout << line;
	}
}

bool Screens::isDarkScreen() const
{
	return screenIsDark[static_cast<int>(current)];
}

bool Screens::isIlluminated(int x, int y, const Player& p1, const Player& p2) const
{
	const int R2 = TORCH_RADIUS * TORCH_RADIUS;
	
	// Check if player 1 has torch and tile is within radius
	if (p1.hasTorch()) {
		int dx = x - p1.getPosition().getX();
		int dy = y - p1.getPosition().getY();
		if (dx * dx + dy * dy <= R2)
			return true;
	}
	
	// Check if player 2 has torch and tile is within radius
	if (p2.hasTorch()) {
		int dx = x - p2.getPosition().getX();
		int dy = y - p2.getPosition().getY();
		if (dx * dx + dy * dy <= R2)
			return true;
	}
	
	return false;
}

// ==========================================
// Board Access & Modification
// ==========================================

void Screens::setCharAt(const Point& p, char ch)
{
	boards[int(current)][p.getY()][p.getX()] = ch;
}

// ==========================================
// Classification Helpers
// ==========================================

bool Screens::isWall(const Point& p) const {
	return (getCharAt(p) == WALL);
}
bool Screens::isDoor(const Point& p) const {
	const char ch = getCharAt(p);
	return (ch >= '1' && ch <= '9');
}
bool Screens::isKey(const Point& p) const {
	return (getCharAt(p) == KEY);
}
bool Screens::isBomb(const Point& p) const {
	return (getCharAt(p) == BOMB);
}
bool Screens::isTorch(const Point& p) const {
	return (getCharAt(p) == TORCH);
}
bool Screens::isObstacle(const Point& p) const {
	return (getCharAt(p) == OBSTACLE);
}
bool Screens::isSwitch(const Point& p) const {
	const char ch = getCharAt(p);
	return (ch == SWITCH_ON || ch == SWITCH_OFF);
}
bool Screens::isSwitchOn(const Point& p) const {
	return (getCharAt(p) == SWITCH_ON);
}
bool Screens::isSwitchOff(const Point& p) const {
	return (getCharAt(p) == SWITCH_OFF);
}
bool Screens::isRiddle(const Point& p) const {
	return (getCharAt(p) == RIDDLE);
}
bool Screens::isHint(const Point& p) const {
	return (getCharAt(p) == HINT);
}
bool Screens::isunbreakable_wall(const Point& p) const
{
	return (getCharAt(p) == UNBREAKABLE_WALL);
}

bool Screens::isFreeCellForPlayer(const Point& p) const
{
	if (!isInside(p)) return false;
	if (isWall(p) || isObstacle(p) || isDoor(p) || isunbreakable_wall(p))
		return false;
	return true;
}

// ==========================================
// Switch & Gate Logic
// ==========================================

void Screens::setSwitchOn(const Point& p) {
	setCharAt(p, SWITCH_ON);
}

void Screens::setSwitchOff(const Point& p) {
	setCharAt(p, SWITCH_OFF);
}

void Screens::makePassage(const Point& p) {
	setCharAt(p, EMPTY_SPACE);
}

void Screens::updateSwitchStates(const Player& p1, const Player& p2)		//We used chatGPT for this function
{
	std::vector<SwitchData>* switches = nullptr;

	if (isFirstScreen()) {
		switches = &firstScreenSwitches;
	}
	else if (isSecondScreen()) {
		switches = &SecondScreenSwitches;
	}
	else {
		return;
	}

	for (auto& s : *switches)
	{
		bool on = (p1.getPosition() == s.position || p2.getPosition() == s.position);

		if (!s.isPermanent)
		{
			if (on && !s.active)
			{
				s.active = true;
				applySwitchEffect(s, true);
				setSwitchOn(s.position);

				for (const Point& b : s.autobombs)
					pendingAutoBombs.push_back(b);
			}
			else if (!on && s.active)
			{
				s.active = false;
				applySwitchEffect(s, false);
				setSwitchOff(s.position);
			}
		}
		else
		{
			if (s.oneTime)
			{
				
				if (on && !s.wasOnLastFrame && !s.active)
				{
					s.active = true;
					applySwitchEffect(s, true);
					setSwitchOn(s.position);

					for (const Point& b : s.autobombs)
						pendingAutoBombs.push_back(b);
				}
			}
			else
			{
				
				if (on && !s.wasOnLastFrame)
				{
					s.active = !s.active;
					applySwitchEffect(s, s.active);

					if (s.active)
					{
						setSwitchOn(s.position);
						for (const Point& b : s.autobombs)
							pendingAutoBombs.push_back(b);
					}
					else
					{
						setSwitchOff(s.position);
					}
				}
			}

			s.wasOnLastFrame = on;
		}
	}

	if (isSecondScreen())
	{
		updateDoor7ByBinaryPuzzle();
	}
}

// --- Level 1 Specifics ---

void Screens::initFirstScreenSwitches()
{
	firstScreenSwitches.clear();

	{
		SwitchData s;
		s.position = Point(50, 4);
		s.isPermanent = false;
		s.oneTime = false;
		s.bitIndex  = -1;
		int y = 4;
		for (int x = 52; x <= 60; ++x)
			s.affectedWalls.emplace_back(x, y);
		for (int x = 62; x <= 78; ++x)
			s.affectedWalls.emplace_back(x, y);
		firstScreenSwitches.push_back(s);
	}

	{
		SwitchData s;
		s.position = Point(50, 8);
		s.isPermanent = false;
		s.oneTime = false;
		s.bitIndex = -1;
		int x = 61;
		for (int y = 1; y <= 3; ++y)
			s.affectedWalls.emplace_back(x, y);
		for (int y = 5; y <= 9; ++y)
			s.affectedWalls.emplace_back(x, y);
		firstScreenSwitches.push_back(s);
	}

	{
		SwitchData s;
		s.position = Point(50, 12);
		s.isPermanent = false;
		s.oneTime = false;
		s.bitIndex = -1;
		int y = 12;
		for (int x = 52; x <= 60; ++x)
			s.affectedWalls.emplace_back(x, y);
		for (int x = 62; x <= 78; ++x)
			s.affectedWalls.emplace_back(x, y);
		firstScreenSwitches.push_back(s);
	}

	{
		SwitchData s;
		s.position = Point(67, 14);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = -1;
		s.affectedWalls.emplace_back(51, 13);
		s.affectedWalls.emplace_back(51, 14);
		s.affectedWalls.emplace_back(61, 13);
		s.affectedWalls.emplace_back(61, 14);
		firstScreenSwitches.push_back(s);
	}
}

void Screens::updateFirstScreenGates(const Player& p1, const Player& p2)
{
	handleGateForPlayer(p1);
	handleGateForPlayer(p2);
}

// --- Level 2 Specifics ---

void Screens::initSecondScreenSwitches()
{
	SecondScreenSwitches.clear();
	{
		SwitchData s;
		s.position = Point(77, 9);
		s.isPermanent = true;
		s.oneTime = true;
		s.bitIndex = -1;
		int row = 10;
		for (int x = 25; x <= 28; ++x)
			s.affectedWalls.emplace_back(x, row);
		row = 14;
		for (int x = 24; x <= 29; ++x)
			s.addWhenActive.emplace_back(x, row);
		s.autobombs.emplace_back(29, 11);
		s.autobombs.emplace_back(29, 13);
		s.autobombs.emplace_back(24, 11);
		s.autobombs.emplace_back(24, 13);
		s.autobombs.emplace_back(24, 9);
		s.autobombs.emplace_back(29, 9);
		SecondScreenSwitches.push_back(s);
	}
	{
		SwitchData s;
		s.position = Point(77, 4);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = -1;
		int row = 14;
		for (int x = 25; x <= 28; ++x)
			s.affectedWalls.emplace_back(x, row);
		SecondScreenSwitches.push_back(s);
	}

	const int y = 16;
	{
		SwitchData s;
		s.position = Point(59, y);  
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 3;
		SecondScreenSwitches.push_back(s);
	}
	{
		SwitchData s;
		s.position = Point(63, y);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 2;
		SecondScreenSwitches.push_back(s);
	}
	{
		SwitchData s;
		s.position = Point(68, y);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 1;
		SecondScreenSwitches.push_back(s);
	}
	{
		SwitchData s;
		s.position = Point(73, y); 
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 0;
		SecondScreenSwitches.push_back(s);
	}
	{
		SwitchData s;
		s.position = Point(29, 19); 
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = -1;         
		s.affectedWalls.emplace_back(72, 4);
		SecondScreenSwitches.push_back(s);
	}
	{
		SwitchData s;
		s.position = Point(52, 2);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = -1;
		s.affectedWalls.emplace_back(22, 1);
		s.affectedWalls.emplace_back(22, 2);
		SecondScreenSwitches.push_back(s);
	}
}

void Screens::printHint() const
{
	if (!isSecondScreen())
		return;
	gotoxy(25, 22);
	std::cout << "Hint: The door password is shown but in decimal";
}

void Screens::clearHint() const
{
	if (!isSecondScreen())
		return;
	gotoxy(40, 22);
	std::cout << "                                                          ";
}

// ==========================================
// Obstacle Mechanics
// ==========================================

bool Screens::tryPushObstacle(const Point& nextPos, const Player& player, const Player& otherPlayer)	// we used chatGPT for this function
{
	Point currPos = player.getPosition();
	int dx = nextPos.getX() - currPos.getX();
	int dy = nextPos.getY() - currPos.getY();

	if (std::abs(dx) + std::abs(dy) != 1)
		return false;

	std::vector<Point> group;
	collectObstacleGroup(nextPos, group);

	if (group.empty())
		return false;

	int obstacleSize = (int)group.size();
	int availableForce = 1;

	Point otherPos = otherPlayer.getPosition();
	Point otherNext = otherPos;
	otherNext.move();

	int odx = otherNext.getX() - otherPos.getX();
	int ody = otherNext.getY() - otherPos.getY();

	bool sameDir = (odx == dx && ody == dy);

	if (sameDir)
	{
		// Check if the other player is behind to add pushing force
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
				if (isObstacle(otherNext))
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

		if (!isInside(target))
			return false;

		if (target.getX() == otherPos.getX() &&
			target.getY() == otherPos.getY())
		{
			return false;
		}

		if (isWall(target) || isDoor(target) || isunbreakable_wall(target))
			return false;

		if (isObstacle(target))
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

	// Move all connected obstacles simultaneously
	moveObstacleGroup(group, dx, dy);
	return true;
}

// ==========================================
// Bomb & Explosion Logic
// ==========================================

void Screens::placeBombAt(int x, int y)									
{
	int s = static_cast<int>(current);
	boards[s][y][x] = BOMB;
	gotoxy(x, y);
	std::cout << BOMB;
}

void Screens::clearExplosionArea(const Point& center, int radius)
{
	int screen = static_cast<int>(current);
	int cx = center.getX();
	int cy = center.getY();
	int r2 = radius * radius;

	for (int dy = -radius; dy <= radius; ++dy)       // we used chatGPT for this loop
	{
		for (int dx = -radius; dx <= radius; ++dx)
		{
			if (dx * dx + dy * dy > r2)
				continue;

			int x = cx + dx;
			int y = cy + dy;

			if (x <= 0 || x >= MAX_X - 1 || y <= 0 || y >= MAX_Y - 1)
				continue;

			char& tile = boards[screen][y][x];

			if (tile == WALL || tile == OBSTACLE || tile == BOMB)
			{
				tile = EMPTY_SPACE;
			}
		}
	}
}

void Screens::collectPendingAutoBombs(std::vector<Point>& out)						// we used chatGPT for this function
{
	out.insert(out.end(), pendingAutoBombs.begin(), pendingAutoBombs.end());
	pendingAutoBombs.clear();
}

// ==========================================
// Internal Helpers
// ==========================================

// Legacy build functions - now use file loading
void Screens::buildFirstScreen()
{
	if (!screenFilePaths.empty())
	{
		loadScreenFromFile(static_cast<int>(ScreenId::First), screenFilePaths[0]);
	}
}

void Screens::buildSecondScreen()
{
	if (screenFilePaths.size() > 1)
	{
		loadScreenFromFile(static_cast<int>(ScreenId::Second), screenFilePaths[1]);
		door7Pos.setPosition(71, 11);
	}
}

void Screens::buildFinalScreen()
{
	if (screenFilePaths.size() > 2)
	{
		loadScreenFromFile(static_cast<int>(ScreenId::Final), screenFilePaths[2]);
	}
}

bool Screens::isInside(const Point& p) const {
	return (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y);
}

char Screens::getCharAt(const Point& p) const {
	return boards[int(current)][p.getY()][p.getX()];
}

void Screens::applySwitchEffect(const SwitchData& s, bool active)
{
	int screenIndex = static_cast<int>(current);

	for (const auto& wall : s.affectedWalls)
	{
		int x = wall.getX();
		int y = wall.getY();
		if (!isInside(Point(x, y)))
			continue;
		char& tile = boards[screenIndex][y][x];
		tile = active ? EMPTY_SPACE : WALL;
	}
	for (const auto& p : s.addWhenActive)
	{
		int x = p.getX();
		int y = p.getY();
		if (!isInside(Point(x, y)))
			continue;
		char& tile = boards[screenIndex][y][x];
		tile = active ? WALL : EMPTY_SPACE;
	}
}

void Screens::triggerAutoBombs(const SwitchData& s)						// we used chatGPT for this function
{
	const int RADIUS = 2; 

	for (const Point& center : s.autobombs)
	{
		clearExplosionArea(center, RADIUS);
		
		int screenIndex = static_cast<int>(current);
		int x = center.getX();
		int y = center.getY();
		if (isInside(center))
		{
			char& tile = boards[screenIndex][y][x];
			if (tile == AUTO_BOMB)
				tile = EMPTY_SPACE;
		}
	}
}

void Screens::updateDoor7ByBinaryPuzzle()							// we used chatGPT for this function
{
	if (!isSecondScreen())
		return;

	if (door7Pos.getX() < 0 || door7Pos.getY() < 0)
		return;

	int value = 0;

	for (const auto& s : SecondScreenSwitches)				
	{
		// Build integer value from active switches (bits 0-3)
		if (s.bitIndex >= 0 && s.active)								
		{
			value |= (1 << s.bitIndex);
		}
	}

	char& tile = boards[(int)current][door7Pos.getY()][door7Pos.getX()];

	if (value == 7)
	{
		tile = EMPTY_SPACE;
	}
	else
	{
		tile = '7';
	}
}

void Screens::handleGateForPlayer(const Player& p)
{
	const Point& pos = p.getPosition();
	int x = pos.getX();
	int y = pos.getY();
	if (!leftEntranceClosed &&
		(y == 18) &&
		(x == 11 || x == 12))
	{
		leftGateClosed();
	}

	if (!rightEntranceClosed &&
		(y == 18) &&
		(x == 16 || x == 17))
	{
		rightGateClosed();
	}
}

void Screens::leftGateClosed()
{
	if (leftEntranceClosed || !isFirstScreen())
		return;
	leftEntranceClosed = true;
	int screenIndex = static_cast<int>(current);
	boards[screenIndex][18][11] = WALL;
	boards[screenIndex][18][12] = WALL;
}

void Screens::rightGateClosed()
{
	if (rightEntranceClosed || !isFirstScreen())
		return;
	rightEntranceClosed = true;
	int screenIndex = static_cast<int>(current);
	boards[screenIndex][18][16] = WALL;
	boards[screenIndex][18][17] = WALL;
}

void Screens::collectObstacleGroup(const Point& start, std::vector<Point>& group) const		// we used chatGPT for this function
{
	const Direction dirs[4] = {
	Direction::UP,
	Direction::RIGHT,
	Direction::DOWN,
	Direction::LEFT
	};

	group.clear();

	if (!isObstacle(start))
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

			if (!isInside(neighbour))
				continue;

			if (!isObstacle(neighbour))
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

void Screens::moveObstacleGroup(const std::vector<Point>& group, int dx, int dy)			// we used chatGPT for this function
{
	for (size_t i = 0; i < group.size(); ++i)
		setCharAt(group[i], Screens::EMPTY_SPACE);

	for (size_t i = 0; i < group.size(); ++i)
	{
		Point cell = group[i];
		Point target(cell.getX() + dx, cell.getY() + dy, 0, 0, ' ');
		setCharAt(target, Screens::OBSTACLE);
	}
}
