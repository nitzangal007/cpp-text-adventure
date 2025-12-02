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
		 "W                                  @               W         W                 W", // 5
		 "W                                                  W         W                 W", // 6
		 "W                                                  W         W                 W", // 7
		 "W                                                 \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 8
		 "W                                                  W         W                 W", // 9
		 "W                                                  W         W                 W", // 10
		 "W                                                  W         W                 W", // 11
		 "W                                      K          \\WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 12
		 "W                                                  W         W                 W", // 13
		 "W                                                  W         W        K        W", // 14
		 "W                                                  WWWW   WWWWWWWWWWWWWWWWWWWWWW", // 15
		 "W                                                   J W   W                    W", // 16
		 "W                                        W   W        W   W                    W", // 17
		 "W                                        W   W        W   W                    W", // 18
		 "W                                        W   W                                 W", // 19
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 20 door '1'
		 "                                                                                ", // 21
		 "                                                                                ", // 22
		 "                                                                                ", // 23
		 "                                                                                "  // 24 
	};
	constexpr const char* SECOND_SCREEN_TEMPLATE[Screens::MAX_Y] = {
   //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	    "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
	   	"WWWWWWW	\\	*	\\	wwwwwwwwwwWWWWWWWWWWWWWWWWWWWWWWwwwwwwwwwwwwwwwwwwwwwwwwW", // 1
	   	"Wwwwwwwww		*		wwwwwwwwwwWWWWWWWWWWWWWWWWWWWWWWwwwwwwwwwwwwwwwwwwwwwwwwW", // 2
	   	"Wwwwwwwww		*		****  @ ww			wwww		www			@	  w		W", // 3
	   	"Wwwwwwwww		*		****	ww		4	wwww	3	www				  w	  \\W", // 4
	   	"Wwwwwwwww	    	        wwwwwwwwww    		wwww		www**ww*ww**www	    W", // 5
	   	"Wwwwwwwww		*	    	wwwwwwwwww			wwww		www**ww*ww**www     W", // 6
	   	"Wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww		wwwwwwwwwwwwwwwwwww	  \\W", // 7
	   	"WWWWWWWWWWWWWWWWwwwwwwww			   wwwwwwwww		wwwwwwwwwwwwwwwwwww     W", // 8
	   	"WW   *     *  kWwwwwwwwwWWWWWWWwwwwwwwwwwwwwwwwwwwwwwwww			w			W", // 9
	   	"WW   W     W   Wwwwwwwww		wwwwwwwwwwwwwwwwwwwwwwww		?	w			W", // 10
	   	"WW *   ***     Wwwwwwwww@	     @wwwwwwwwwwwwwwwwwwwwwwwwwwww*wwwwwwwwwww*wwwwwW", // 11
	   	"WW   W   W   * Wwwwwwwww		wwwwwwwwwwwwwwwwwwwwwwww						W", // 12
	   	"WW   W   W   W Wwwwwwwww@	   @wwwwwwwwwwwwwwwwwwwwwwww H						W", // 13
	   	"WW *   *   *   WwwwwwwwwWWWWWWWwwwwwwwwwwwwwwwwwwwwwwww				        W", // 14
	   	"WW   W   W   W Wwwwwwwww		wwwwwwwwwwwwwwwwwwwwwwww						W", // 15
	   	"WW             Wwwwwwwww		wwwwwwwwwwwwwwwwwwwwwwww						W", // 16
	   	"WWWWWWWW WWWWWWWwwwwwwww       wwwwwwwwwwwwwwwwwwwwwwww	\\ \\     \\ \\ 	W", // 17
	   	"W							  \\wwwwwwwwwwwwwwwwwwwwwwww						W", // 18
	   	"W							    wwwwwwwwwwwwwwwwwwwwwwww						W", // 19
	   	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 20
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
