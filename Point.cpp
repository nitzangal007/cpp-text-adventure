#include "Point.h"											//Point.cpp 
#include "Screens.h"										// We took the code from class


void Point::move() {
	x = (x + diff_x + Screens::MAX_X) % Screens::MAX_X;
	y = (y + diff_y + Screens::MAX_Y) % Screens::MAX_Y;
}

void Point::setDirection(Direction dir) {
	switch (dir) {
	case Direction::UP:
		diff_x = 0;
		diff_y = -1;
		break;
	case Direction::RIGHT:
		diff_x = 1;
		diff_y = 0;
		break;
	case Direction::DOWN:
		diff_x = 0;
		diff_y = 1;
		break;
	case Direction::LEFT:
		diff_x = -1;
		diff_y = 0;
		break;
	case Direction::STAY:
		diff_x = 0;
		diff_y = 0;
		break;
	}
}
																		