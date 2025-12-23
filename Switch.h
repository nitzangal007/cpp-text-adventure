#pragma once

#include "Point.h"
#include "GameConstants.h"
#include <vector>

/**
 * Switch class - handles pressure plate / switch behavior
 * Owns: state machine (permanent/toggle/one-time), effect application, auto-bomb triggers
 * 
 * Switch types:
 * - Temporary (!isPermanent): Active only while player stands on it
 * - Permanent toggle (isPermanent && !oneTime): Toggles on each step
 * - One-time (isPermanent && oneTime): Activates once and stays on
 */
class Switch {
public:
    // Configuration (set at construction, read-only after)
    Point position;
    std::vector<Point> affectedWalls;   // Walls to remove when active
    std::vector<Point> addWhenActive;   // Walls to add when active
    std::vector<Point> autobombs;       // Auto-bombs to trigger when activated
    bool isPermanent = false;           // If true, state persists after stepping off
    bool oneTime = false;               // If true, can only activate once
    int bitIndex = -1;                  // For binary puzzle (-1 = not used)

private:
    // Runtime state (managed internally)
    bool active_ = false;               // Current activation state
    bool wasOnLastFrame_ = false;       // For edge detection (toggle behavior)
    std::vector<Point> pendingTriggers_; // Pending triggers for this frame

public:
    // Constructors
    Switch() = default;
    
    /**
     * Construct a switch with basic configuration
     * @param pos Switch position on the board
     * @param permanent If true, state persists after stepping off
     * @param isOneTime If true, can only activate once
     * @param bitIdx For binary puzzle (-1 = not used)
     */
    Switch(const Point& pos, bool permanent, bool isOneTime = false, int bitIdx = -1)
        : position(pos), isPermanent(permanent), oneTime(isOneTime), bitIndex(bitIdx) {}

    /**
     * Update switch state based on player position
     * @param playerOnSwitch true if any player is standing on this switch
     * @param boardModifier Function to modify board: void(Point, char)
     * @return true if state changed this frame
     */
    template<typename BoardModifier>
    bool update(bool playerOnSwitch, BoardModifier setCharAt);

    /**
     * Apply switch effect (add/remove walls)
     * @param activate true to activate effect, false to deactivate
     * @param boardModifier Function to modify board: void(Point, char)
     */
    template<typename BoardModifier>
    void applyEffect(bool activate, BoardModifier setCharAt);

    /**
     * Get and clear pending auto-bomb triggers
     * @return vector of positions where auto-bombs should be spawned
     */
    std::vector<Point> getAndClearPendingTriggers();

    /**
     * Update the visual representation on the board
     * @param boardModifier Function to modify board: void(Point, char)
     */
    template<typename BoardModifier>
    void updateVisual(BoardModifier setCharAt);

    // Getters
    bool isActive() const { return active_; }
    const Point& getPosition() const { return position; }
    int getBitIndex() const { return bitIndex; }
};

// Template implementations
template<typename BoardModifier>
bool Switch::update(bool playerOnSwitch, BoardModifier setCharAt)
{
    bool stateChanged = false;

    if (!isPermanent)
    {
        // Temporary switch: active only while player is on it
        if (playerOnSwitch && !active_)
        {
            active_ = true;
            applyEffect(true, setCharAt);
            updateVisual(setCharAt);
            
            // Queue auto-bombs
            for (const Point& b : autobombs)
                pendingTriggers_.push_back(b);
            
            stateChanged = true;
        }
        else if (!playerOnSwitch && active_)
        {
            active_ = false;
            applyEffect(false, setCharAt);
            updateVisual(setCharAt);
            stateChanged = true;
        }
    }
    else
    {
        // Permanent switch
        if (oneTime)
        {
            // One-time: activate on first step, never deactivate
            if (playerOnSwitch && !wasOnLastFrame_ && !active_)
            {
                active_ = true;
                applyEffect(true, setCharAt);
                updateVisual(setCharAt);
                
                for (const Point& b : autobombs)
                    pendingTriggers_.push_back(b);
                
                stateChanged = true;
            }
        }
        else
        {
            // Toggle: flip state each time player steps on
            if (playerOnSwitch && !wasOnLastFrame_)
            {
                active_ = !active_;
                applyEffect(active_, setCharAt);
                
                if (active_)
                {
                    for (const Point& b : autobombs)
                        pendingTriggers_.push_back(b);
                }
                
                updateVisual(setCharAt);
                stateChanged = true;
            }
        }
        
        wasOnLastFrame_ = playerOnSwitch;
    }

    return stateChanged;
}

template<typename BoardModifier>
void Switch::applyEffect(bool activate, BoardModifier setCharAt)
{
    if (activate)
    {
        // Remove affected walls
        for (const Point& w : affectedWalls)
            setCharAt(w, Tiles::EMPTY_SPACE);
        
        // Add walls when active
        for (const Point& w : addWhenActive)
            setCharAt(w, Tiles::WALL);
    }
    else
    {
        // Restore affected walls
        for (const Point& w : affectedWalls)
            setCharAt(w, Tiles::WALL);
        
        // Remove added walls
        for (const Point& w : addWhenActive)
            setCharAt(w, Tiles::EMPTY_SPACE);
    }
}

template<typename BoardModifier>
void Switch::updateVisual(BoardModifier setCharAt)
{
    if (active_)
        setCharAt(position, Tiles::SWITCH_ON);
    else
        setCharAt(position, Tiles::SWITCH_OFF);
}
