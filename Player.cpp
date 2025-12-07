#include "Player.h"
#include <iostream>

// Move the player according to its current direction
void Player::move()
{
	pos.draw(' '); // erase previous position
	pos.move();
}

// Handle a key press: if it matches one of this player's control keys, set direction
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

// reset player to starting position and state
void Player::reset(const Point& startPos)
{
    pos = startPos;
    pos.setDirection(Direction::STAY);
	removeHeldItem();
    
}
