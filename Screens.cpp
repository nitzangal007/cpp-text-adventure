#include "Screens.h"

namespace
{
	// 80ª25 template for the first screen (ScreenId::First)
	constexpr const char* FIRST_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W                 WWWWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 1
		 "W     *   *                                                  W                 W", // 2
		 "W                 WWWWWWWWWWWWWWWWWWWWWWWWWWWW WWWWW         W                 W", // 3
		 "W          J      WWWW @ WWWWWWWWWWWWWWWWW        \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 4
		 "W                 WWWW   WWWWWWWWWWWWWWWWW         W         W                 W", // 5
		 "W                 WWWW   WWWWWWWWWWWWWWWWW         W         W                 W", // 6
		 "W        *        WWWW   WWWWWWWWWWWWWWWWW         W         W                 W", // 7
		 "WWWWWWWWW*WWWWWWWWWWWW   WWWWWWWWWWWWWWWWW        \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 8
		 "W         W  *          WWWWWWWWWWWWWWWWWW         W         W                 W", // 9
		 "W         W  WWWWWWWWWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 10
		 "W         W  WWW  *  WWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 11
		 "W         W  WWW  W WWWWWWWWWWWWWWWWWWWWWW        \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 12
		 "W         W  WWW  W WWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 13
		 "W         W  WWW  W                                W         W        K        W", // 14
		 "W         W  WWW  WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW   WWWWWWWWWWWWWWWWWWWWWW", // 15
		 "W         W  WWW  WWWWWWWWWWWWWWWWWWWWWWWW            W   W                    W", // 16
		 "W         W  WWW  WWWWWWWWWWWWWWWWWWWWWWWW   W        W   W                    W", // 17
		 "W         W  WWW  WWWWWWWWWWWWWWWWWWWWWWWW   W        W   W                    W", // 18
		 "W         ?       WWWWWWWWWWWWWWWWWWWWWWWW   W                                 W", // 19
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 20 door '1'
		 "                                                                                ", // 21
		 "                                                                                ", // 22
		 "                                                                                ", // 23
		 "                                                                                "  // 24 
	};
	constexpr const char* SECOND_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789

		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "WWWWWWW       *       WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 1
		 "WWWWWWW               WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 2
		 "WWWWWWW       *       ****  @ WW          WWWW        WWW         @     w     WW", // 3
		 "WWWWWWW       *       ****    WW      4   WWWW    3   WWW               *     WW", // 4
		 "WWWWWWW               wwwwwwwwww          WWWW        WWW***WW***WW***WWW     WW", // 5
		 "WWWWWWW       *       wwwwwwwwww          WWWW        WWW***WW***WW***WWW     WW", // 6
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW        WWWWWWWWWWWWWWWWWWW     WW", // 7
		 "WWWWWWWWWWWWWWWWWWWWWWWW               WWWWWWW        WWWWWWWWWWWWWWWWWWW     WW", // 8
		 "WW   *     *   WWWWWWWWW               WWWWWWWWWWWWWWW             W          WW", // 9
		 "WW   W     W   WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW      ?      W          WW", // 10
		 "WW *   ***     WWWWWWWWW@    @WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW**WWW**WWWWWWWWW", // 11
		 "WW   W   W   * WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 12
		 "WW   W   W   W WWWWWWWWW@    @WWWWWWWWWWWWWWWWWWWWWWWW H                      WW", // 13   // NUMBER 4
		 "WW *   *   *   WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 14
		 "WW   W   W    WWWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 15
		 "WW             WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW        //   //   //    WW", // 16
		 "WWWWWWWW WWWWWWWwwwwwwww      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 17
		 "W                             WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 18
		 "W                             WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 19
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 20 door '1'
		 "                                                                                ", // 21
		 "                                                                                ", // 22
		 "                                                                                ", // 23
		 "                                                                                "  // 24

	};
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

		s.affectedWalls.emplace_back(51, 13);
		s.affectedWalls.emplace_back(51, 14);
		s.affectedWalls.emplace_back(61, 13);
		s.affectedWalls.emplace_back(61, 14);
		firstScreenSwitches.push_back(s);
	}



}

void Screens::updateSwitchStates(const Player& p1, const Player& p2)
{
	if (!isFirstScreen())
		return;

	for (auto& s : firstScreenSwitches)
	{
		bool on = (p1.getPosition() == s.position || p2.getPosition() == s.position);

		if (!s.isPermanent)
		{
			if (on && !s.active)
			{
				s.active = true;
				applySwitchEffect(s, true);
				setSwitchOn(s.position);
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

			if (on && !s.wasOnLastFrame)
			{
				s.active = !s.active;
				applySwitchEffect(s, s.active);
				if (s.active)
					setSwitchOn(s.position);
				else
					setSwitchOff(s.position);
			}

			s.wasOnLastFrame = on;
		}
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







Screens::Screens() = default;

void Screens::init()
{
    // For now only first screen is meaningful; we still build all three
    buildFirstScreen();
    buildSecondScreen();
  //  buildFinalScreen();
	current = ScreenId::First;

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
}
