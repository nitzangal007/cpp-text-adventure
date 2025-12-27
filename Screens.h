#pragma once
#include "Player.h"
#include "utils.h"
#include "GameConstants.h"
#include "Switch.h"
#include "Spring.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <filesystem>

class Screens
{
public:
    // ==========================================
    // Constants & Enums
    // ==========================================
    enum { MAX_X = Screen::MAX_X, MAX_Y = Screen::MAX_Y };
    enum { NUM_SCREENS = Screen::NUM_SCREENS };
    enum class ScreenId { First = 0, Second = 1, Final = 2 };

    // Note: Tile constants moved to GameConstants.h - use Tiles:: namespace directly
    // Example: Tiles::WALL, Tiles::KEY, Radius::TORCH_LIGHT, etc.


private:
    char boards[NUM_SCREENS][MAX_Y][MAX_X];
    ScreenId current = ScreenId::First;
    std::vector<Point> pendingAutoBombs;
    std::vector<Spring> screenSprings[NUM_SCREENS];

    // Level 1 Data (now using Switch class)
    std::vector<Switch> firstScreenSwitches;
    bool leftEntranceClosed = false;
    bool rightEntranceClosed = false;

    // Level 2 Data (now using Switch class)
    std::vector<Switch> SecondScreenSwitches;
    Point door7Pos;

    // Darkness per screen (Screen 2 is dark)
    bool screenIsDark[NUM_SCREENS] = { false, false, false };
    
    // File loading data
    std::vector<std::string> screenFilePaths;    // Discovered .screen file paths
    int legendY[NUM_SCREENS] = { -1, -1, -1 };   // Y position of 'L' anchor per screen
    bool loadingFailed = false;                   // True if screen loading failed
    std::string loadingError;                     // Error message if loading failed

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
    
    // Draw with torch illumination (for dark screens)
    void drawCurrentWithTorch(const Player& p1, const Player& p2) const;
    
    // Check if screen is dark
    bool isDarkScreen() const;
    
    // Get legend Y position for current screen (where status bar starts)
    int getLegendY() const;
    
    // Check if loading failed and get error message
    bool hasLoadingError() const { return loadingFailed; }
    const std::string& getLoadingError() const { return loadingError; }

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
	bool isSpring(const Point& p) const;
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

    // Attempt to push an obstacle with specified force
    // pushDir: direction of push (UP/DOWN/LEFT/RIGHT)
    // primaryForce: force from pushing player (1=normal, launchSpeed=spring)
    // otherPlayer: check for cooperative pushing
    bool tryPushObstacle(const Point& nextPos, Direction pushDir,
                         int primaryForce, const Player& otherPlayer);

    // ==========================================
    // Bomb & Explosion Logic
    // ==========================================

    void placeBombAt(int x, int y);

    // Clears area around center (destroys walls/obstacles/bombs)
    void clearExplosionArea(const Point& center, int radius);

    // Collects auto-bombs triggered this frame
    void collectPendingAutoBombs(std::vector<Point>& out);

    // ==========================================
    // Spring Mechanics
    // ==========================================

    // Initializes springs for all screens (calls scanSpringsForScreen internally)
    void initSprings();

    // Returns a pointer to the spring at point p (or nullptr if none)
    Spring* getSpringAt(const Point& p);

    // Helper for drawing: Should '#' be drawn?
    bool shouldDrawSpringChar(const Point& p, const Player& p1, const Player& p2) const;

    // Inside class Screens (Private Helper):
    void scanSpringsForScreen(int screenIndex);


private:
    // ==========================================
    // Internal Helpers
    // ==========================================

    // Screen file loading
    bool discoverScreenFiles();                              // Find adv-world*.screen files
    bool loadScreenFromFile(int screenIndex, const std::string& path);  // Load single screen
    
    // Legacy build functions (now call loadScreenFromFile internally)
    void buildFirstScreen();
    void buildSecondScreen();
    void buildFinalScreen();

    // Helpers
    bool isInside(const Point& p) const;
    char getCharAt(const Point& p) const;
    bool isIlluminated(int x, int y, const Player& p1, const Player& p2) const;

    // Switch logic (handled by Switch class update method)
    void updateDoor7ByBinaryPuzzle();

    // Gates (Level 1)
    void handleGateForPlayer(const Player& p);
    void leftGateClosed();
    void rightGateClosed();

    // Obstacles
    void collectObstacleGroup(const Point& start, std::vector<Point>& group) const;
    void moveObstacleGroup(const std::vector<Point>& group, int dx, int dy);

    // Spring scanning helpers
    Direction getOppositeDirection(Direction dir) const;
    void collectSpringChain(int screenIndex, const Point& start,
                            bool visited[MAX_Y][MAX_X], std::vector<Point>& chain);
    bool findSpringAnchor(int screenIndex, const std::vector<Point>& chain,
                          Point& outAnchorWall, Direction& outPushDir, Direction& outReleaseDir);
};