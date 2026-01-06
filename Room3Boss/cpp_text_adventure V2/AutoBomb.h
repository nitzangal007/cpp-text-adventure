#pragma once

#include "Point.h"
#include "GameConstants.h"

// Forward declarations
class Screens;

/**
 * AutoBomb class - handles switch-triggered automatic bombs
 * Owns: timer, explosion, blinking animation
 */
class AutoBomb {
public:
    Point center;
    int ticksLeft = 0;

    // Note: Constants from GameConstants.h - use AUTO_BOMB_DELAY_TICKS, BOMB_EXPLOSION, AUTO_BOMB_BLINK_THRESHOLD directly

    // Constructor
    AutoBomb() = default;
    AutoBomb(const Point& pos) : center(pos), ticksLeft(AUTO_BOMB_DELAY_TICKS) {}

    /**
     * Tick the auto-bomb timer
     * @return true if bomb should explode this frame
     */
    bool tick();

    /**
     * Execute explosion - clears area around bomb
     * @param screen The screen to apply explosion to
     */
    void explode(Screens& screen);

    /**
     * Check if bomb should be blinking (for render)
     * @return true if bomb should show blank space this frame
     */
    bool shouldBlinkOff() const;

    /**
     * Get the character to display at bomb position
     * @return AUTO_BOMB char or EMPTY_SPACE if blinking off
     */
    char getDisplayChar() const;

    // Getters
    const Point& getPosition() const { return center; }
    int getRadiusSquared() const { return BOMB_EXPLOSION * BOMB_EXPLOSION; }
    bool isExpired() const { return ticksLeft <= 0; }
};
