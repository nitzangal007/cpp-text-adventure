#include "Screens.h"

namespace
{
	// 80ª25 template for the first screen (ScreenId::First)
	constexpr const char* FIRST_SCREEN_TEMPLATE[Screens::MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W                                                  W                           W", // 1
		 "W                                                  W                           W", // 2
		 "W                                                  W                           W", // 3
		 "W                                                 \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 4
		 "W                                                  W         W                 W", // 5
		 "W                                                  W         W                 W", // 6
		 "W                                                  W         W                 W", // 7
		 "W                                                 \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 8
		 "W                                                  W         W                 W", // 9
		 "W                                                  W         W                 W", // 10
		 "W                                                  W         W                 W", // 11
		 "W                                                 \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 12
		 "W                                                  W         W                 W", // 13
		 "W                                                  W         W        K        W", // 14
		 "W                                                  WWWW   WWWWWWWWWWWWWWWWWWWWWW", // 15
		 "W                                                   ? W   W                    W", // 16
		 "W                                        W   W        W   W                    W", // 17
		 "W                                        W   W        W   W                    W", // 18
		 "W                                        W   W                                 W", // 19
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
		if(raised)
		{
			if (ch == WALL && (prevToCh == SWITCH_OFF || prevToCh == SWITCH_ON))
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







Screens::Screens() = default;

void Screens::init()
{
    // For now only first screen is meaningful; we still build all three
    buildFirstScreen();
  //  buildSecondScreen();
  //  buildFinalScreen();
	current = ScreenId::First;
	drawCurrent();
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
