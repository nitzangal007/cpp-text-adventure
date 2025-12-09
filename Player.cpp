#include "Player.h"
#include <iostream>


void Player::move()                                                         //we took this function from class
{
	pos.draw(' '); 
	pos.move();
}


void Player::handleKeyPress(char key_pressed) {                              //we took this function from class
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
