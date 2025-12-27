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
	
}
void Player::resetSpringState() {
    springState.mode = SpringMode::None;
    springState.springId = -1;
    springState.compressedCount = 0;
    springState.launchSpeed = 0;
    springState.ticksLeft = 0;
    springState.launchDir = Direction::STAY;
}

bool Player::tickFlight()
{
    if (springState.mode != SpringMode::Launching)
        return false;

    springState.ticksLeft--;

    if (springState.ticksLeft <= 0)
    {
        Direction finalDir = springState.launchDir;
        resetSpringState();
        pos.setDirection(finalDir);
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

int Player::computePushForce(Direction moveDir) const
{
	// Spring-boosted force: only when launching AND moving in launch direction
	if (springState.mode == SpringMode::Launching &&
		moveDir == springState.launchDir)
	{
		return springState.launchSpeed;
	}
	// Normal walking force for all other cases
	return 1;
}



// reset player to starting position and state
void Player::reset(const Point& startPos)
{
    pos = startPos;
    pos.setDirection(Direction::STAY);
	removeHeldItem();
    resetSpringState();
}
