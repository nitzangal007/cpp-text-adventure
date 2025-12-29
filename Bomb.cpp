#include "Bomb.h"
#include "Screens.h"
#include "Player.h"

bool Bomb::place(Player& player, Screens& screen)
{
    // Can't place if already active or player doesn't have bomb
    if (active_)
        return false;
    if (!player.hasBomb())
        return false;
    
    // Activate bomb at player position
    active_ = true;
    pos_ = player.getPosition();
    ticksLeft_ = BOMB_FUSE_TICKS;
    player.removeHeldItem();
    
    // Update board
    int x = pos_.getX();
    int y = pos_.getY();
    screen.placeBombAt(x, y);
    
    return true;
}

bool Bomb::tick()
{
    if (!active_)
        return false;
    
    ticksLeft_--;
    return (ticksLeft_ <= 0);
}

void Bomb::explode(Screens& screen)
{
    if (!active_)
        return;
    
    screen.clearExplosionArea(pos_, BOMB_EXPLOSION);
    // Reset by assigning default-constructed Bomb
    *this = Bomb();
}

bool Bomb::shouldBlinkOff() const
{
    if (!active_)
        return false;
    
    // Blink when close to exploding, on odd ticks
    return (ticksLeft_ <= BOMB_BLINK_THRESHOLD) && (ticksLeft_ % 2 != 0);
}

char Bomb::getDisplayChar() const
{
    if (shouldBlinkOff())
        return EMPTY_SPACE;
    return BOMB;
}
