#include "Torch.h"
#include "Player.h"
#include "Screens.h"

void Torch::onPickup(Player& player)
{
    player.collectTorch();
}

void Torch::onDrop(Player& player, Screens& screen)
{
    if (!player.hasTorch())
        return;
    
    // Place torch back on the board at player's current position
    Point pos = player.getPosition();
    screen.setCharAt(pos, Tiles::TORCH);
    player.removeHeldItem();
}

bool Torch::isInRange(const Point& holderPos, const Point& targetPos)
{
    int dx = targetPos.getX() - holderPos.getX();
    int dy = targetPos.getY() - holderPos.getY();
    int R2 = LIGHT_RADIUS * LIGHT_RADIUS;
    
    return (dx * dx + dy * dy) <= R2;
}

bool Torch::playerHasTorch(const Player& player)
{
    return player.hasTorch();
}
