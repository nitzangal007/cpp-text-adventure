#include "Screens.h"
#include <cmath>

namespace
{
	// 80ª25 template for the first screen (ScreenId::First)
	constexpr const char* FIRST_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W                 WWWWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 1
		 "W                 WWWW                                       W                 W", // 2
		 "W                 WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW         W                 W", // 3
		 "W         @       WWWW   WWWWWWWWWWWWWWWWW        \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 4
		 "W                 WWWW   @WWWWWWWWWWWWWWWW         W         W                 W", // 5
		 "W                 WWWW   WWWWWWWWWWWWWWWWW         W         W                 W", // 6
		 "W                 WWWW   WWWWWWWWWWWWWWWWW         W         W                 W", // 7
		 "WWWWWWW**WWWWWWWWWWWWW   WWWWWWWWWWWWWWWWW        \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 8
		 "W         W  *          WWWWWWWWWWWWWWWWWW         W         W                 W", // 9
		 "W  WWWW   W  WWWWWWWWWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 10
		 "W**    *  W  WWW  *  WWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 11
		 "W  *  * **W  WWW  W WWWWWWWWWWWWWWWWWWWWWW        \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 12
		 "W  W  W **W  WWW  W WWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 13
		 "W  W  W   W  WWW  W                                W         W        K        W", // 14
		 "W   ** *  W  WWW  WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW   WWWWWWWWWWWWWWWWWWWWWW", // 15
		 "WWW   W   W  WWW  WWWWWWWWWWWWWWWWWWWWWWWW            W   W                    W", // 16
		 "W     W   W  WWW  WWWWWWWWWWWWWWWWWWWWWWWW   W        W   W                    W", // 17
		 "W  * *W   W  WWW  WWWWWWWWWWWWWWWWWWWWWWWW   W        W   W                    W", // 18
		 "W   *             WWWWWWWWWWWWWWWWWWWWWWWW   W                                 W", // 19
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 20 door '1'
		 "                                                                                ", // 21
		 "                                                                                ", // 22
		 "                                                                                ", // 23
		 "                                                                                "  // 24 
	};
	constexpr const char* SECOND_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789

		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "WWWWWWWWWWWWW         W                          * WWW     WWWWWWWWWWWWWWWWWWWWW", // 1
		 "WWWWWWWWWWW           W         WWWW**WWWWWWWWWW*   \\  WWWWWWWWWWWWWWWWWWWWWWWWW", // 2
		 "WWWWWWWWWWW  WWWWWWWWWWWWWWWWWWWWW       WWWWWWWWWWWWWWWW         @     W     WW", // 3
		 "WWWWWWWWWWW  WWWWWWWWWWWWWWWWWWWW         WWWWWWWWWWWWWWW               W    \\WW", // 4
		 "WWWWWWWWWWW  WWWWWWWWWWWWWWWWWWWWW*W***W*WWWWWWWWWWWWWWWW***WW***WW***WWWW WW*WW", // 5
		 "WWWWWWW           WWWWWWWWWWWWWWWW  @ @      *        WWW***WW***WW***WWW *   WW", // 6
		 "WWWWWWW           WWWWWWWWWWWWWWWW WWWWW WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW *WW", // 7
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW WW", // 8
		 "WWWWWWWWWWWWWWWWWWWWWWWWB    BWWWWWWWWWWWWWWWWWWWWWWWW               W       \\WW", // 9
		 "WW *        *  W WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW               W        WW", // 10
		 "WW***WWW@W W   *  *WWWWWB    BWWWWWW     WWWWWWWWWWWWWWWWWWWWW  WWWWWWW7WWWWWWWW", // 11
		 "WW * W **W W*WWW WWWWWWW      WWWWWW     WWWWWWWWWWWWW                        WW", // 12
		 "WW*W W   W W   *  *WWWWW      WWWWWW  2  WWWWWWWWWWWWW H                      WW", // 13   // NUMBER 4
		 "WW  ** W*W*W* *WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 14
		 "WW*W   W W  *  WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 15
		 "WW   *  W  *   WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW     \\   \\    \\    \\    WW", // 16
		 "WWW WWWWWWWWW WWWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 17
		 "W                             WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 18
		 "W                            \\WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 19
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 20 door '1'
		 "                                                                                ", // 21
		 "                                                                                ", // 22
		 "                                                                                ", // 23
		 "                                                                                "  // 24

	};
	constexpr const char* FINAL_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "################################################################################", // 0
		 "#  ##########################################################################  #", // 1
		 "#      ##################################################################      #", // 2
		 "#        ##############################################################        #", // 3
		 "#        ##                                                          ##        #", // 4
		 "#        ##                                                          ##        #", // 5
		 "#        ##                                                          ##        #", // 6
		 "#        ##                                                          ##        #", // 7
		 "#        ##                                                          ##        #", // 8
		 "#        ##     ***   ***  *   * *****    ***  *   * ***** ****      ##        #", // 9
		 "#        ##    *   * *   * ** ** *       *   * *   * *     *   *     ##        #", // 10
		 "#        ##    *     *   * * * * *       *   * *   * *     *   *     ##        #", // 11
		 "#        ##    * *** ***** *   * ****    *   * *   * ****  ****      ##        #", // 12
		 "#        ##    *   * *   * *   * *       *   *  * *  *     * *       ##        #", // 13
		 "#        ##    *   * *   * *   * *       *   *  * *  *     *  *      ##        #", // 14
		 "#        ##     ***  *   * *   * *****    ***    *   ***** *   *     ##        #", // 15
		 "#        ##                                                          ##        #", // 16
		 "#        ##                                                          ##        #", // 17
		 "#        ##                                                          ##        #", // 18
		 "#        ##                                                          ##        #", // 19
		 "#        ##                                                          ##        #", // 20
		 "#        ##############################################################        #", // 21
		 "#      ##################################################################      #", // 22
		 "#  ##########################################################################  #", // 23
		 "################################################################################", // 24
	};

}





void Screens::clearHint() const
{
	if (!isSecondScreen())
		return;

	gotoxy(40, 22);
	std::cout << "                                                          ";
}


bool Screens::isFreeCellForPlayer(const Point& p) const
{
	if (!isInside(p)) return false;
	if (isWall(p) || isObstacle(p) || isDoor(p))
		return false;
	return true;
}


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

		// 52..60
		for (int x = 52; x <= 60; ++x)
			s.affectedWalls.emplace_back(x, y);

		// 62..68
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

		// rows 1..3
		for (int y = 1; y <= 3; ++y)
			s.affectedWalls.emplace_back(x, y);

		// rows 5..9
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

		// 52..60
		for (int x = 52; x <= 60; ++x)
			s.affectedWalls.emplace_back(x, y);

		// 62..68
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
		s.bitIndex  = -1;
	}

	const int y = 16;

	
	{
		SwitchData s;
		s.position = Point(59, y);  
		s.isPermanent = true;
		s.oneTime = false;
		s.bitIndex = 3;            // MSB
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
		s.bitIndex = 0;            // LSB
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

void Screens::updateSwitchStates(const Player& p1, const Player& p2)
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

		// --- סוויץ' לא-קבוע: עובד רק כל עוד עומדים עליו ---
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
		// --- סוויץ' קבוע (isPermanent == true) ---
		else
		{
			// סוויץ' חד-פעמי
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
			// סוויץ' קבוע רגיל – ON/OFF בכל דריכה
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

	// עדכון דלת 7 לפי החידה – פעם אחת אחרי כל הסוויצ'ים
	if (isSecondScreen())
	{
		updateDoor7ByBinaryPuzzle();
	}
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

void Screens::updateFirstScreenGates(const Player& p1, const Player& p2)
{
	handleGateForPlayer(p1);
	handleGateForPlayer(p2);
}


void Screens::placeBombAt(int x, int y)									
{

	int s = static_cast<int>(current);

	boards[s][y][x] = BOMB;   // '@'
	gotoxy(x, y);
	std::cout << BOMB;

}

void Screens::clearExplosionArea(const Point& center, int radius)
{
	int screen = static_cast<int>(current);
	int cx = center.getX();
	int cy = center.getY();
	int r2 = radius * radius;

	for (int dy = -radius; dy <= radius; ++dy)
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

void Screens::triggerAutoBombs(const SwitchData& s)
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

void Screens::collectPendingAutoBombs(std::vector<Point>& out)
{

	out.insert(out.end(), pendingAutoBombs.begin(), pendingAutoBombs.end());

	
	pendingAutoBombs.clear();
}

void Screens::printHint() const
{
	if (!isSecondScreen())
		return;
	gotoxy(25, 22);
	std::cout << "Hint: The door password is shown but in decimal";
}


void Screens::resetCurrent()
{
	const int screenIndex = static_cast<int>(current);
	// Rebuild the current screen from the template
	if (current == ScreenId::First)
	{
		buildFirstScreen();
		leftEntranceClosed = false;
		rightEntranceClosed = false;

	}
	else if (current == ScreenId::Second)
	{
		buildSecondScreen();
		initSecondScreenSwitches();
	}

}

void Screens::updateDoor7ByBinaryPuzzle()
{
	if (!isSecondScreen())
		return;

	// אם לא מצאנו מיקום לדלת 7 – לא עושים כלום
	if (door7Pos.getX() < 0 || door7Pos.getY() < 0)
		return;

	int value = 0;

	// לוקחים רק סוויצ'ים שיש להם bitIndex תקף
	for (const auto& s : SecondScreenSwitches)
	{
		if (s.bitIndex >= 0 && s.active)
		{
			value |= (1 << s.bitIndex);
		}
	}

	char& tile = boards[(int)current][door7Pos.getY()][door7Pos.getX()];

	if (value == 7)
	{
		// 0111 – פותחים את הדלת
		tile = EMPTY_SPACE;
	}
	else
	{
		// כל קומבינציה אחרת – דלת סגורה עם '7'
		tile = '7';
	}
}





Screens::Screens() = default;

void Screens::init()
{
    // For now only first screen is meaningful; we still build all three
    buildFirstScreen();
    buildSecondScreen();
    buildFinalScreen();
	current = ScreenId::First;
	pendingAutoBombs.clear();

}


void Screens::drawCurrent() const
{
	const int screenIndex = static_cast<int>(current);
	for (int y = 0; y < MAX_Y; ++y)
	{
		gotoxy(0, y);
		for (int x = 0; x < MAX_X; ++x)
		{
			std::cout << boards[screenIndex][y][x];
		}
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

void Screens::rightGateClosed()
{
	if (rightEntranceClosed || !isFirstScreen())
		return;
	rightEntranceClosed = true;
	int screenIndex = static_cast<int>(current);
	boards[screenIndex][18][16] = WALL;
	boards[screenIndex][18][17] = WALL;
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

void Screens::buildFirstScreen()
{
    const int screenIndex = static_cast<int>(ScreenId::First);

    // Copy template into boards[screenIndex]
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            boards[screenIndex][y][x] = FIRST_SCREEN_TEMPLATE[y][x];
        }
    }
}
void Screens::buildSecondScreen()
{
	const int screenIndex = static_cast<int>(ScreenId::Second);
	// Copy template into boards[screenIndex]
	for (int y = 0; y < MAX_Y; ++y)
	{
		for (int x = 0; x < MAX_X; ++x)
		{
			boards[screenIndex][y][x] = SECOND_SCREEN_TEMPLATE[y][x];
		}
	}

	door7Pos.setPosition(71, 11);
}
void Screens::buildFinalScreen()
{
	const int screenIndex = static_cast<int>(ScreenId::Final);
	// Copy template into boards[screenIndex]
	for (int y = 0; y < MAX_Y; ++y)
	{
		for (int x = 0; x < MAX_X; ++x)
		{
			boards[screenIndex][y][x] = FINAL_SCREEN_TEMPLATE[y][x];
		}
	}
}

bool Screens::tryPushObstacle(const Point& nextPos, const Player& player, const Player& otherPlayer)
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

		if (isWall(target) || isDoor(target))
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

	moveObstacleGroup(group, dx, dy);
	return true;
}

void Screens::collectObstacleGroup(const Point& start, std::vector<Point>& group) const
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

void Screens::moveObstacleGroup(const std::vector<Point>& group, int dx, int dy)
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
