#pragma once

#include "GameConstants.h"

// Forward declarations
class Player;

/**
 * Key class - handles key item behavior
 * Owns: symbol, pickup logic, door-opening capability
 */
class Key {
public:
    /**
     * Get the key symbol for display
     */
    static char getSymbol() { return Tiles::KEY; }

    /**
     * Handle player picking up a key
     * @param player The player collecting the key
     */
    static void onPickup(Player& player);

    /**
     * Check if a character represents a door that can be opened
     * @param ch The character to check
     * @return true if ch is a door ('1'-'9')
     */
    static bool canOpenDoor(char ch);

    /**
     * Check if player has a key
     * @param player The player to check
     * @return true if player is holding a key
     */
    static bool playerHasKey(const Player& player);
};
