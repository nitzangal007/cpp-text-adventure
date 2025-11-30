#include "Screens.h"

bool Screens::isFreeCellForPlayer(const Point& p) const
{
	//if (!isInside(p)) return false;
	if (isWall(p) || isObstacle(p) || isDoor(p))
		return false;
	return true;
}



namespace
{
    // 80ª25 template for the first screen (ScreenId::First)
    constexpr const char* FIRST_SCREEN_TEMPLATE[Screens::MAX_Y] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W                                                 WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 1
        "W                                                 W         W                 WW", // 2
        "W                                                 W         W              K  WW", // 3
        "W                                                 W         W                 WW", // 4
        "W                                                 W         W                 WW", // 5
        "W                        *                        W WWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 6
        "W                                                 W         W                 WW", // 7
        "W     *  *  *  *  *                               W         W                 WW", // 8
        "W                        *                        W         W                 WW", // 9
        "W                                                 W         W                 WW", // 10
        "W                                                 W         W                 WW", // 11
        "W           *   *   *   **  *                     W         W                 WW", // 12
        "W                                                 W         W                 WW", // 13
        "W                                                 W         W                 WW", // 14
        "W                                                 WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 15
        "W                                                                              W", // 16
        "W                                                                              W", // 17
        "W         \\                                                                   W", // 18  switch OFF
        "W                                                                     \\       W", // 19
        "W                                         ?                                    W", // 20
        "W                                       *   *                                  W", // 21
        "W                                       *   *                                  W", // 22
        "W                                       *   *                                  W", // 23
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24 door '1'
    };
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
