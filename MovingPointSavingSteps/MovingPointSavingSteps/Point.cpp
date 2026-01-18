#include "Point.h"
#include "Board.h"

bool Point::keyPressed(char key) {
	for (size_t i = 0; i < numKeys; i++) {
		if (std::tolower(key) == keys[i]) {
			dir = directions[i];
			return true; // key is relevant and used
		}
	}
	return false; // uninteresting key
}

bool Point::move() {
	int newX = x + dir.x;
	int newY = y + dir.y;
	// Better use a function in Board to check if the new position is valid
	// + Better use a constant for the wall character
	if (pBoard->getChar(newX, newY) == 'W') {
		dir = { 0, 0 };
	}
	else {
		x = newX;
		y = newY;
	}
	// BAD Design, quick and dirty...
	if (pBoard->getChar(newX, newY) == '@') {
		return false; // hit a bomb... (bad design, shouldn't be checked here...)
	}
	return true;
}
