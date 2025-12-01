#include "Player.h"
#include <iostream>


void Player::move()
{
	pos.draw(' '); // erase previous position
	pos.move();
}

void Player::handleKeyPress(char key_pressed) {
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(k) == std::tolower(key_pressed)) {
			setDirection((Direction)index);
            return;
        }
        ++index;
    }
}

void Player::reset(const Point& startPos)
{
    pos = startPos;
    pos.setDirection(Direction::STAY);

    hasKeyFlag = false;
    hasBombFlag = false;
    hasTorchFlag = false;
}
