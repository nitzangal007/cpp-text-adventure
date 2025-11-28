#include "Screens.h"

bool Screens::isFreeCellForPlayer(const Point& p) const
{
	if (!isInside(p)) return false;
	if (isWall(p) || isObstacle(p) || isDoor(p))
		return false;
	return true;
}

void Screens::buildFirstScreen()
{
	
}
