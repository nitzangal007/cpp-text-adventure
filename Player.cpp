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

void Player::handleSpringEntry(int springId)
{
	springState.mode = SpringMode::Compressing;
	springState.springId = springId;
	springState.compressedCount = 0;
}

void Player::launch(const SpringLaunchParams& params, Direction dir)
{
	springState.mode = SpringMode::Launching;
	springState.launchSpeed = params.speed;
	springState.ticksLeft = params.durationTicks;
	pos.setDirection(dir);
	springState.launchDir = dir;
	pos.setDirection(dir);
}

bool Player::tickFlight()
{
    if (springState.mode != SpringMode::Launching)
        return false;

    springState.ticksLeft--;

    if (springState.ticksLeft <= 0)
    {
        springState.mode = SpringMode::None;
        pos.setDirection(Direction::STAY); 
        return true; 
    }

    return false; 
}

void Player::absorbMomentum(const SpringState& otherState)
{
	if (otherState.mode == SpringMode::Launching)
	{
		launch(
			SpringLaunchParams{ otherState.launchSpeed, otherState.ticksLeft },
			otherState.launchDir);
	}
}



// reset player to starting position and state
void Player::reset(const Point& startPos)
{
    pos = startPos;
    pos.setDirection(Direction::STAY);
	removeHeldItem();
    
}
