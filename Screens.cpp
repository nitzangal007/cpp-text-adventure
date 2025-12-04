#include "Screens.h"

namespace
{
	// 80ª25 template for the first screen (ScreenId::First)
	constexpr const char* FIRST_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W                 WWWWWWWWWWWWWWWWWWWWWWWW         W         W                 W", // 1
		 "W                 WWWW                                       W                 W", // 2
		 "W                 WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW         W                 W", // 3
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
		 "WWWWWWW   \\     *     WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 1
		 "WWWWWWW               WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 2
		 "WWWWWWW       *       ****  @ WW          WWWW        WWW         @     w     WW", // 3
		 "WWWWWWW       *       ****    WW      4   WWWW    3   WWW               *   \\ WW", // 4
		 "WWWWWWW               wwwwwwwwww          WWWW        WWW**WW*WW**WWW     WW", // 5
		 "WWWWWWW       *       wwwwwwwwww          WWWW        WWW**WW*WW**WWW     WW", // 6
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW       WWWWWWWWWWWWWWWWWWW    \\ WW", // 7
		 "WWWWWWWWWWWWWWWWWWWWWWWW               WWWWWW        WWWWWWWWWWWWWWWWWWW      WW", // 8
		 "WW   *     *   WWWWWWWWW                WWWWWWWWWWWWWWW           W           WW", // 9
		 "WW   W     W   WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWWW      ?    W           WW", // 10
		 "WW *   ***     WWWWWWWWW@    @WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW*WWW*WWWWWWWWW", // 11
		 "WW   W   W   * WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 12
		 "WW   W   W   W WWWWWWWWW@    @WWWWWWWWWWWWWWWWWWWWWWWW H                      WW", // 13   // NUMBER 4
		 "WW *   *   *   WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 14
		 "WW   W   W    WWWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW                        WW", // 15
		 "WW             WWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWW   \\   \\   \\    \\       WW", // 16
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

bool Screens::hasSwitchInRow(int row) const
{
	int screenIndex = static_cast<int>(current);
	for (int x = 0; x < MAX_X; ++x)
	{
		const char ch = boards[screenIndex][row][x];
		if (ch == SWITCH_ON || ch == SWITCH_OFF)
			return true;
	}
	return false;
}

void Screens::setRowWallsRaised(int row, bool raised)
{
	if (!isFirstScreen())
		return;
	int screenIndex = static_cast<int>(current);
	for (int x = 1; x < MAX_X - 1; ++x)
	{
		const char ch = FIRST_SCREEN_TEMPLATE[row][x];
		const char prevToCh = FIRST_SCREEN_TEMPLATE[row][x-1];
		const char benathToCh = FIRST_SCREEN_TEMPLATE[row+1][x];
		if(raised)
		{
			if (ch == WALL && (prevToCh == SWITCH_OFF || prevToCh == SWITCH_ON || benathToCh == WALL))
				continue;
			if (ch == WALL)
				boards[screenIndex][row][x] = EMPTY_SPACE;
		}
		else
		{
			if (ch == WALL)
				boards[screenIndex][row][x] = WALL;
		}
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
    const int screenIndex = static_cast<int>(ScreenId::Second);

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
	const int screenIndex = static_cast<int>(ScreenId::First);
	// Copy template into boards[screenIndex]
	for (int y = 0; y < MAX_Y; ++y)
	{
		for (int x = 0; x < MAX_X; ++x)
		{
			boards[screenIndex][y][x] = SECOND_SCREEN_TEMPLATE[y][x];
		}
	}
}
