#pragma once

#include "Point.h"
#include "GameConstants.h"

// Forward declarations
class Screens;
class Player;

/**
 * Bomb class - handles player-placed bombs
 * Owns: placement, fuse timer, explosion, blinking animation
 */
class Bomb {
private:
    // State
    bool active_ = false;
    Point pos_;
    int ticksLeft_ = 0;

public:
    // Constants
    static constexpr int FUSE_TICKS = Timing::BOMB_FUSE_TICKS;
    static constexpr int RADIUS = Radius::BOMB_EXPLOSION;
    static constexpr int BLINK_THRESHOLD = Timing::BOMB_BLINK_THRESHOLD;

    // Constructors
    Bomb() = default;
    
    // Core operations
    
    /**
     * Place a bomb at the player's current position
     * @param player The player placing the bomb
     * @param screen The screen to place bomb on
     * @return true if bomb was placed, false if already active or player has no bomb
     */
    bool place(Player& player, Screens& screen);
    
    /**
     * Tick the bomb timer
     * @return true if bomb should explode this frame
     */
    bool tick();
    
    /**
     * Execute explosion - clears area around bomb
     * @param screen The screen to apply explosion to
     */
    void explode(Screens& screen);
    
    // Visual
    
    /**
     * Check if bomb should be blinking (for render)
     * @return true if bomb should show blank space this frame
     */
    bool shouldBlinkOff() const;
    
    /**
     * Get the character to display at bomb position
     * @return BOMB char or EMPTY_SPACE if blinking off
     */
    char getDisplayChar() const;
    
    // Getters
    bool isActive() const { return active_; }
    const Point& getPosition() const { return pos_; }
    int getRadiusSquared() const { return RADIUS * RADIUS; }
};
