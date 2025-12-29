#include "Screens.h"
#include "ColorUtils.h"
#include <cmath>

Screens::Screens() = default;

// ==========================================
// Screen File Loading
// ==========================================

bool Screens::discoverScreenFiles()
{
	namespace fs = std::filesystem;
	screenFilePaths.clear();

	for (const auto& entry : fs::directory_iterator(fs::current_path()))
	{
		auto filename = entry.path().filename();
		auto filenameStr = filename.string();

		if (filenameStr.size() >= 9 &&
			filenameStr.substr(0, 9) == "adv-world" && 
			filename.extension() == ".screen")
		{
			screenFilePaths.push_back(filenameStr);
		}
	}

	std::sort(screenFilePaths.begin(), screenFilePaths.end());

	return !screenFilePaths.empty();
}

bool Screens::loadScreenFromFile(int screenIndex, const std::string& filename)
{
	std::ifstream screen_file(filename);

	if (!screen_file.is_open())
	{
		loadingError = "Cannot open file: " + filename;
		loadingFailed = true;
		return false;
	}

	legendY[screenIndex] = -1;

	int curr_row = 0;
	int curr_col = 0;
	char c;

	while (!screen_file.get(c).eof() && curr_row < MAX_Y)
	{
		if (c == '\n')
		{
			while (curr_col < MAX_X)
			{
				boards[screenIndex][curr_row][curr_col++] = ' ';
			}
			++curr_row;
			curr_col = 0;
			continue;
		}

		if (curr_col < MAX_X)
		{
			if (c == LEGEND_ANCHOR)
			{
				legendY[screenIndex] = curr_row;
				boards[screenIndex][curr_row][curr_col++] = EMPTY_SPACE;
			}
			else
			{
				boards[screenIndex][curr_row][curr_col++] = c;
			}
		}
	}

	while (curr_col < MAX_X && curr_row < MAX_Y)
	{
		boards[screenIndex][curr_row][curr_col++] = ' ';
	}

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
			return;  //loadingError already set
		}
	}
	
	// Step 4: Initialize screen-specific data (switches, door positions, etc.)
	door7Pos.setPosition(71, 11);  // Set door7 position for screen 2
	initFirstScreenSwitches();
	initSecondScreenSwitches();
	
	// Step 5: Scan and register all springs
	initSprings();

	initRiddles();

	 
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
	const bool dark = screenIsDark[screenIndex];

	
	const bool partialOn = (partialDarkEnabled[screenIndex] && partialX1[screenIndex] != -1);
	const int x1 = partialX1[screenIndex];
	const int y1 = partialY1[screenIndex];
	const int x2 = partialX2[screenIndex];
	const int y2 = partialY2[screenIndex];

	// When colors disabled, use fast buffered output
	if (!g_colorsEnabled)
	{
		for (int y = 0; y < MAX_Y; ++y)
		{
			gotoxy(0, y);
			std::string line;
			line.reserve(MAX_X);

			for (int x = 0; x < MAX_X; ++x)
			{
				const char cell = boards[screenIndex][y][x];
				const bool inPartial = (partialOn && x >= x1 && x <= x2 && y >= y1 && y <= y2);

				if ((dark || inPartial) && !isIlluminated(x, y, p1, p2) && cell != TORCH)
					line += DARKNESS_CHAR;
				else
					line += cell;
			}

			std::cout << line;
		}
		return;
	}

	// Colors enabled: batch consecutive same-color characters
	for (int y = 0; y < MAX_Y; ++y)
	{
		gotoxy(0, y);
		std::string buffer;
		buffer.reserve(MAX_X);

		ConsoleColor currentColor = ConsoleColor::Default;
		bool colorSet = false;

		for (int x = 0; x < MAX_X; ++x)
		{
			const char cell = boards[screenIndex][y][x];
			const bool inPartial = (partialOn && x >= x1 && x <= x2 && y >= y1 && y <= y2);

			char displayChar;
			ConsoleColor charColor;

			if ((dark || inPartial) && !isIlluminated(x, y, p1, p2) && cell != TORCH)
			{
				displayChar = DARKNESS_CHAR;
				charColor = ConsoleColor::Default;
			}
			else
			{
				displayChar = cell;
				charColor = getColorForChar(cell);
			}

			// If color changed, flush buffer and switch color
			if (charColor != currentColor || !colorSet)
			{
				if (!buffer.empty())
				{
					std::cout << buffer;
					buffer.clear();
				}

				setConsoleColor(charColor);
				currentColor = charColor;
				colorSet = true;
			}

			buffer += displayChar;
		}

		// Flush remaining buffer for this line
		if (!buffer.empty())
		{
			std::cout << buffer;
		}
	}

	// Reset to default at end
	resetColor();
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
bool Screens::isSpring(const Point& p) const
{
	return (getCharAt(p) == SPRING);
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

void Screens::updateSwitchStates(const Player& p1, const Player& p2)
{
	std::vector<Switch>* switches = nullptr;

	if (isFirstScreen()) {
		switches = &firstScreenSwitches;
	}
	else if (isSecondScreen()) {
		switches = &SecondScreenSwitches;
	}
	else {
		return;
	}

	// Lambda to modify board characters
	auto setCharAtLambda = [this](const Point& p, char ch) {
		this->setCharAt(p, ch);
	};

	for (auto& s : *switches)
	{
		bool on = (p1.getPosition() == s.position || p2.getPosition() == s.position);
		
		// Use Switch class update method
		s.update(on, setCharAtLambda);
		
		// Collect pending auto-bomb triggers
		std::vector<Point> triggers = s.getAndClearPendingTriggers();
		for (const Point& b : triggers)
			pendingAutoBombs.push_back(b);
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
		Switch s;
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
		Switch s;
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
		Switch s;
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
		Switch s;
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
		Switch s;
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
		Switch s;
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
		Switch s;
		s.position = Point(59, y);  
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 3;
		SecondScreenSwitches.push_back(s);
	}
	{
		Switch s;
		s.position = Point(63, y);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 2;
		SecondScreenSwitches.push_back(s);
	}
	{
		Switch s;
		s.position = Point(68, y);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 1;
		SecondScreenSwitches.push_back(s);
	}
	{
		Switch s;
		s.position = Point(73, y); 
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 0;
		SecondScreenSwitches.push_back(s);
	}
	{
		Switch s;
		s.position = Point(29, 19); 
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = -1;         
		s.affectedWalls.emplace_back(72, 4);
		SecondScreenSwitches.push_back(s);
	}
	{
		Switch s;
		s.position = Point(52, 2);
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = -1;
		s.affectedWalls.emplace_back(22, 1);
		s.affectedWalls.emplace_back(22, 2);
		SecondScreenSwitches.push_back(s);
	}
}


void Screens::initRiddles()
{
	// ????? – ???? ??????
	addRiddle(ScreenId::Third,
		Riddle(
			Point(22, 2),
			"What is 2+2?",
			std::array<std::string, 4>{"3", "4", "5", "22"},
			1
		)
	);

	addRiddle(ScreenId::Third,
		Riddle(
			Point(56, 2),
			"Which key drops a bomb for Player 1?",
			std::array<std::string, 4>{"E", "O", "R", "H"},
			0
		)
	);
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

bool Screens::tryPushObstacle(const Point& nextPos, Direction pushDir,
                               int primaryForce, const Player& otherPlayer)
{
	// Compute push deltas from direction
	int dx = 0, dy = 0;
	switch (pushDir)
	{
		case Direction::UP:    dy = -1; break;
		case Direction::DOWN:  dy =  1; break;
		case Direction::LEFT:  dx = -1; break;
		case Direction::RIGHT: dx =  1; break;
		default: return false;
	}

	// Validate single-cell movement
	if (std::abs(dx) + std::abs(dy) != 1)
		return false;

	// Collect all connected obstacles (BFS flood-fill)
	std::vector<Point> group;
	collectObstacleGroup(nextPos, group);

	if (group.empty())
		return false;

	int obstacleSize = static_cast<int>(group.size());
	
	// Start with primary player's force (may be spring-boosted)
	int availableForce = primaryForce;

	// Check if other player is cooperating (same direction push)
	Point otherPos = otherPlayer.getPosition();
	Point otherNext = otherPos;
	otherNext.move();

	int odx = otherNext.getX() - otherPos.getX();
	int ody = otherNext.getY() - otherPos.getY();
	bool sameDir = (odx == dx && ody == dy);

	if (sameDir)
	{
		// Determine other player's movement direction
		Direction otherMoveDir = Direction::STAY;
		if (ody < 0) otherMoveDir = Direction::UP;
		else if (ody > 0) otherMoveDir = Direction::DOWN;
		else if (odx < 0) otherMoveDir = Direction::LEFT;
		else if (odx > 0) otherMoveDir = Direction::RIGHT;
		
		// Get other player's force (may also be spring-boosted)
		int otherForce = otherPlayer.computePushForce(otherMoveDir);

		// Derive pushing player's position from obstacle position
		Point pusherPos(nextPos.getX() - dx, nextPos.getY() - dy);
		
		// Case 1: Other player is directly BEHIND the pusher
		// This happens after momentum transfer - sender pushes through receiver
		bool otherBehind = (otherPos.getX() == pusherPos.getX() - dx) &&
		                   (otherPos.getY() == pusherPos.getY() - dy);
		
		if (otherBehind)
		{
			availableForce += otherForce;
		}
		// Case 2: Other player is pushing same obstacle group directly
		else if (isObstacle(otherNext))
		{
			bool inSameGroup = false;
			for (const Point& cell : group)
			{
				if (cell.getX() == otherNext.getX() &&
					cell.getY() == otherNext.getY())
				{
					inSameGroup = true;
					break;
				}
			}

			if (inSameGroup)
			{
				// Both players pushing same group - add forces
				availableForce += otherForce;
			}
		}
	}

	// Force check: can we push this group?
	if (availableForce < obstacleSize)
		return false;

	// Validate all destination cells
	for (const Point& cell : group)
	{
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
			for (const Point& g : group)
			{
				if (g.getX() == target.getX() && g.getY() == target.getY())
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

void Screens::initSprings()
{
	for (int i = 0; i < NUM_SCREENS; ++i)
	{
		scanSpringsForScreen(i);
	}
}

Spring* Screens::getSpringAt(const Point& p)
{
	if (!isSpring(p))
		return nullptr;
	int screenIndex = static_cast<int>(current);
	for (auto& spring : screenSprings[screenIndex])
	{
		if (spring.contains(p))
			return &spring;
	}
	return nullptr;
}

bool Screens::shouldDrawSpringChar(const Point& p, const Player& p1, const Player& p2) const
{
	// TODO: Implement if needed for spring compression visual feedback
	return true;
}

// ==========================================
// Spring Scanning Implementation
// ==========================================

Direction Screens::getOppositeDirection(Direction dir) const
{
	switch (dir)
	{
		case Direction::UP:    return Direction::DOWN;
		case Direction::DOWN:  return Direction::UP;
		case Direction::LEFT:  return Direction::RIGHT;
		case Direction::RIGHT: return Direction::LEFT;
		default:               return Direction::STAY;
	}
}

void Screens::collectSpringChain(
	int screenIndex,
	const Point& start,
	bool visited[MAX_Y][MAX_X],
	std::vector<Point>& chain)
{
	// BFS to collect all connected '#' cells in any direction
	chain.clear();
	chain.push_back(start);
	visited[start.getY()][start.getX()] = true;

	// Movement deltas for 4 directions
	const int dx[] = { 0, 1, 0, -1 };  // UP, RIGHT, DOWN, LEFT
	const int dy[] = { -1, 0, 1, 0 };

	for (size_t i = 0; i < chain.size(); ++i)
	{
		Point current = chain[i];

		// Check all 4 neighbors
		for (int d = 0; d < 4; ++d)
		{
			Point neighbor(current.getX() + dx[d], current.getY() + dy[d]);

			if (!isInside(neighbor))
				continue;

			if (boards[screenIndex][neighbor.getY()][neighbor.getX()] != SPRING)
				continue;

			if (visited[neighbor.getY()][neighbor.getX()])
				continue;

			visited[neighbor.getY()][neighbor.getX()] = true;
			chain.push_back(neighbor);
		}
	}
}

bool Screens::findSpringAnchor(
	int screenIndex,
	const std::vector<Point>& chain,
	Point& outAnchorWall,
	Direction& outPushDir,
	Direction& outReleaseDir)
{
	const Direction directions[] = {
		Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT
	};
	const int dx[] = { 0, 1, 0, -1 };
	const int dy[] = { -1, 0, 1, 0 };

	// Check each cell in the chain for an adjacent wall
	for (const Point& cell : chain)
	{
		for (int d = 0; d < 4; ++d)
		{
			Point neighbor(cell.getX() + dx[d], cell.getY() + dy[d]);

			if (!isInside(neighbor))
				continue;

			char neighborChar = boards[screenIndex][neighbor.getY()][neighbor.getX()];

			if (neighborChar == WALL)
			{
				// Found anchor wall!
				outAnchorWall = neighbor;
				outPushDir = directions[d];
				outReleaseDir = getOppositeDirection(outPushDir);
				return true;
			}
		}
	}

	return false;  // No anchor found
}

void Screens::scanSpringsForScreen(int screenIndex)
{
	// Step 1: Clear existing springs for this screen
	screenSprings[screenIndex].clear();

	// Visited matrix to track processed spring cells
	bool visited[MAX_Y][MAX_X] = { false };
	int springId = 0;

	// Step 2: Scan every cell on the board
	for (int y = 0; y < MAX_Y; ++y)
	{
		for (int x = 0; x < MAX_X; ++x)
		{
			// Skip if not a spring char or already processed
			if (boards[screenIndex][y][x] != SPRING || visited[y][x])
				continue;

			// Step 3: Collect the ENTIRE chain first (all connected '#' cells)
			std::vector<Point> chain;
			collectSpringChain(screenIndex, Point(x, y), visited, chain);

			// Step 4: Find which cell in the chain has an adjacent anchor wall
			Point anchorWall;
			Direction pushDir, releaseDir;

			if (!findSpringAnchor(screenIndex, chain, anchorWall, pushDir, releaseDir))
				continue;  // Invalid spring (no wall anchor)

			// Step 5: Create Spring and add all segments
			Spring newSpring(springId++, anchorWall, pushDir, releaseDir);

			for (const Point& segment : chain)
				newSpring.addSegment(segment);

			// Step 6: Add to the screen's spring list
			screenSprings[screenIndex].push_back(newSpring);
		}
	}
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

// applySwitchEffect and triggerAutoBombs functions have been removed
// These are now handled by the Switch class's update() and applyEffect() methods

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
		if (s.bitIndex >= 0 && s.isActive())								
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
		setCharAt(group[i], EMPTY_SPACE);

	for (size_t i = 0; i < group.size(); ++i)
	{
		Point cell = group[i];
		Point target(cell.getX() + dx, cell.getY() + dy, 0, 0, ' ');
		setCharAt(target, OBSTACLE);
	}
}

void Screens::addRiddle(ScreenId screen, const Riddle& r)
{
	int idx = static_cast<int>(screen);
	riddlesByScreen[idx].push_back(r);
}

Riddle* Screens::getRiddleAt(const Point& p)
{
	const int screenIndex = static_cast<int>(current);
	auto& vec = riddlesByScreen[screenIndex];

	for (auto& r : vec)
	{
		if (r.getPosition().getX() == p.getX() && r.getPosition().getY() == p.getY())
			return &r;
	}
	return nullptr;
}


bool Screens::removeRiddleAt(const Point& p)
{
	const int screenIndex = static_cast<int>(current);
	auto& vec = riddlesByScreen[screenIndex];

	const auto oldSize = vec.size();

	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[&](const Riddle& r)
			{
				return r.getPosition().getX() == p.getX() &&
					r.getPosition().getY() == p.getY();
			}),
		vec.end()
	);

	const bool removed = (vec.size() != oldSize);
	if (removed)
		makePassage(p); // ???? ?? ?-'?' ????? ???? ??????

	return removed;
}
void Screens::clearRiddles(ScreenId screen)
{
	const int screenIndex = static_cast<int>(screen);
	riddlesByScreen[screenIndex].clear();
}
