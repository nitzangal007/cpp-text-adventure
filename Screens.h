#pragma once
#include "Player.h"
#include "utils.h"
#include <iostream>
#include <vector>

class Screens
{
public:
    // ==========================================
    // Constants & Enums
    // ==========================================
    enum { MAX_X = 80, MAX_Y = 25 };
    enum { NUM_SCREENS = 3 };
    enum class ScreenId { First = 0, Second = 1, Final = 2 };

    // Tile characters
    static constexpr char EMPTY_SPACE = ' ';
    static constexpr char WALL = 'W';
    static constexpr char KEY = 'K';
    static constexpr char BOMB = '@';
    static constexpr char TORCH = '!';
    static constexpr char OBSTACLE = '*';
    static constexpr char SWITCH_ON = '/';
    static constexpr char SWITCH_OFF = '\\';
    static constexpr char RIDDLE = '?';
    static const char BOMB_PLANTED = '@';
    static const char AUTO_BOMB = 'B';
    static const char HINT = 'H';
    static const char  unbreakable_wall = 'X';

    struct SwitchData
    {
        Point position;
        std::vector<Point> affectedWalls;
        std::vector<Point> addWhenActive;
        std::vector<Point> autobombs;
        bool isPermanent;
        bool active = false;
        bool wasOnLastFrame = false;
        bool oneTime = false;
        int bitIndex = -1;
        
        SwitchData() {}
    };

private:
    char boards[NUM_SCREENS][MAX_Y][MAX_X];
    ScreenId current = ScreenId::First;
    std::vector<Point> pendingAutoBombs;

    // Level 1 Data
    std::vector<SwitchData> firstScreenSwitches;
    bool leftEntranceClosed = false;
    bool rightEntranceClosed = false;

    // Level 2 Data
    std::vector<SwitchData> SecondScreenSwitches;
    Point door7Pos;

public:
    Screens();

    // ==========================================
    // General Screen Management
    // ==========================================
    
    // Initialize all screens (loads templates, resets switches)
    void init();

    // Rebuilds the current screen from template
    void resetCurrent();

    // Switch to a specific screen
    void setCurrentScreen(ScreenId id);
    ScreenId getCurrentScreen() const;

    // Draw the entire board to console
    void drawCurrent() const;

    // ==========================================
    // Board Access & Modification
    // ==========================================

    void setCharAt(const Point& p, char ch);

    // ==========================================
    // Classification Helpers (Is Wall? etc.)
    // ==========================================
    
    bool isWall(const Point& p) const;
    bool isDoor(const Point& p) const;
    bool isKey(const Point& p) const;
    bool isBomb(const Point& p) const;
    bool isTorch(const Point& p) const;
    bool isObstacle(const Point& p) const;
    bool isSwitch(const Point& p) const; 
    bool isSwitchOn(const Point& p) const;
    bool isSwitchOff(const Point& p) const;
    bool isRiddle(const Point& p) const;
    bool isHint(const Point& p) const;
    bool isunbreakable_wall(const Point& p) const;

    // Checks if a cell is walkable (not wall/obstacle/door)
    bool isFreeCellForPlayer(const Point& p) const;

    // Screen identity checks
    bool isFirstScreen() const { return (current == ScreenId::First); }
    bool isSecondScreen() const { return (current == ScreenId::Second); }
    bool isFinalScreen() const { return (current == ScreenId::Final); }

    // ==========================================
    // Switch & Gate Logic
    // ==========================================

    void setSwitchOn(const Point& p);
    void setSwitchOff(const Point& p);
    void makePassage(const Point& p); 

    // Update state of all switches based on player positions
    void updateSwitchStates(const Player& p1, const Player& p2);

    // --- Level 1 Specifics ---
    void initFirstScreenSwitches();
    void updateFirstScreenGates(const Player& p1, const Player& p2);

    // --- Level 2 Specifics ---
    void initSecondScreenSwitches();
    void printHint() const;
    void clearHint() const;

    // ==========================================
    // Obstacle Mechanics
    // ==========================================
    
    // Attempt to push an obstacle (single or group)
    bool tryPushObstacle(const Point& nextPos, const Player& player, const Player& otherPlayer);

    // ==========================================
    // Bomb & Explosion Logic
    // ==========================================

    void placeBombAt(int x, int y);

    // Clears area around center (destroys walls/obstacles/bombs)
    void clearExplosionArea(const Point& center, int radius);

    // Collects auto-bombs triggered this frame
    void collectPendingAutoBombs(std::vector<Point>& out);

private:
    // ==========================================
    // Internal Helpers
    // ==========================================

    void buildFirstScreen();
    void buildSecondScreen();
    void buildFinalScreen();

    // Helpers
    bool isInside(const Point& p) const;
    char getCharAt(const Point& p) const;

    // Switch logic
    void applySwitchEffect(const SwitchData& s, bool active);
    void triggerAutoBombs(const SwitchData& s);
    void updateDoor7ByBinaryPuzzle(); 

    // Gates (Level 1)
    void handleGateForPlayer(const Player& p);
    void leftGateClosed();
    void rightGateClosed();

    // Obstacles
    void collectObstacleGroup(const Point& start, std::vector<Point>& group) const;
    void moveObstacleGroup(const std::vector<Point>& group, int dx, int dy);
};