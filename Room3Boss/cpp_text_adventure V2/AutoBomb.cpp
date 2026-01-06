#include "AutoBomb.h"
#include "Screens.h"

bool AutoBomb::tick()
{
    if (ticksLeft > 0)
    {
        --ticksLeft;
    }
    return (ticksLeft <= 0);
}

void AutoBomb::explode(Screens& screen)
{
    screen.clearExplosionArea(center, BOMB_EXPLOSION);
    screen.setCharAt(center, EMPTY_SPACE);
}

bool AutoBomb::shouldBlinkOff() const
{
    // Blink when close to exploding, on odd ticks
    return (ticksLeft <= AUTO_BOMB_BLINK_THRESHOLD) && (ticksLeft % 2 != 0);
}

char AutoBomb::getDisplayChar() const
{
    if (shouldBlinkOff())
        return EMPTY_SPACE;
    return AUTO_BOMB;
}
