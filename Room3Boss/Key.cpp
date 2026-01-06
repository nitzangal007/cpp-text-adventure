#include "Key.h"
#include "Player.h"

void Key::onPickup(Player& player)
{
    player.collectKey();
}

bool Key::canOpenDoor(char ch)
{
    return (ch >= '1' && ch <= '9');
}

bool Key::playerHasKey(const Player& player)
{
    return player.hasKey();
}
