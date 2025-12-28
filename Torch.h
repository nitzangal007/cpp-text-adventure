#pragma once

#include "Point.h"
#include "GameConstants.h"

// Forward declarations
class Player;
class Screens;

/**
 * Torch class - handles torch item behavior
 * Owns: symbol, pickup/drop logic, illumination calculation
 */
class Torch {
public:
    // Constants
    static constexpr int LIGHT_RADIUS = Radius::TORCH_LIGHT;

    /**
     * Get the torch symbol for display
     */
    static char getSymbol() { return Tiles::TORCH; }

    /**
     * Handle player picking up a torch
     * @param player The player collecting the torch
     */
    static void onPickup(Player& player);

    /**
     * Handle player dropping the torch
     * @param player The player dropping the torch
     * @param screen The screen to place torch on
     */
    static void onDrop(Player& player, Screens& screen);

    /**
     * Check if a position is within illumination range of torch holder
     * @param holderPos Position of the player holding the torch
     * @param targetPos Position to check for illumination
     * @return true if targetPos is within LIGHT_RADIUS of holderPos
     */
    static bool isInRange(const Point& holderPos, const Point& targetPos);

    /**
     * Check if player has a torch
     * @param player The player to check
     * @return true if player is holding a torch
     */
    static bool playerHasTorch(const Player& player);
};
