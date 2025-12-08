#pragma once
#include "Player.h"
#include "utils.h"
#include <iostream>
#include <vector>

class Screens
{
public:
    enum { MAX_X = 80, MAX_Y = 25 };
    enum { NUM_SCREENS = 3 };
    enum class ScreenId { First = 0, Second = 1, Final = 2 };

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


    // Doors: '1'..'9' (no constant needed, we check by range)
private:
    // Raw boards for all screens
    char boards[NUM_SCREENS][MAX_Y][MAX_X];

    // Which one we are showing/using now
    ScreenId current = ScreenId::First;
    Point door7Pos;
    //data for first screen
    std::vector<SwitchData> firstScreenSwitches;
    bool leftEntranceClosed = false;
    bool rightEntranceClosed = false;
    void handleGateForPlayer(const Player& p);
    void leftGateClosed();
    void rightGateClosed();


    //data for Second screen
    std::vector<SwitchData> SecondScreenSwitches;

    // Internal builders for each screen
    void buildFirstScreen();
    void buildSecondScreen();
    void buildFinalScreen();
    // Internal checkers

    std::vector<Point> pendingAutoBombs;
public:
    Screens();

    // Build all screens (hard-coded for Exercise 1)
    void init();

    // Current screen management
    void setCurrentScreen(ScreenId id) {
        current = id;
    }
    ScreenId getCurrentScreen() const {
        return current;
    }

    // Draw the currently active screen
    void drawCurrent() const;

    // ---- Board access (current screen) ----

    bool isInside(const Point& p) const {
        return (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y);
    }
    char getCharAt(const Point& p) const {
        return boards[int(current)][p.getY()][p.getX()];
    }
    void setCharAt(const Point& p, char ch) {
        boards[int(current)][p.getY()][p.getX()] = ch;
    }
    

    // ---- Classification helpers ----

    bool isWall(const Point& p) const {
        return (getCharAt(p) == WALL);
    }
    bool isDoor(const Point& p) const {
        const char ch = getCharAt(p);
        return (ch >= '1' && ch <= '9');
    }
    bool isKey(const Point& p) const {
        return (getCharAt(p) == KEY);
    }
    bool isBomb(const Point& p) const {
        return (getCharAt(p) == BOMB);
    }
    bool isTorch(const Point& p) const {
        return (getCharAt(p) == TORCH);
    }
    bool isObstacle(const Point& p) const {
        return (getCharAt(p) == OBSTACLE);
    }
    bool isSwitch(const Point& p) const {
        const char ch = getCharAt(p);
        return (ch == SWITCH_ON || ch == SWITCH_OFF);
    }
    bool isSwitchOn(const Point& p) const {
        return (getCharAt(p) == SWITCH_ON);
    }
    bool isSwitchOff(const Point& p) const {
        return (getCharAt(p) == SWITCH_OFF);
    }
    bool isRiddle(const Point& p) const {
        return (getCharAt(p) == RIDDLE);
    }
	bool isHint(const Point& p) const {
		return (getCharAt(p) == HINT);
	}

    void clearHint() const;

    // Screen identity helpers
    bool isFirstScreen() const {
        return (current == ScreenId::First);
    }
    bool isSecondScreen() const {
        return (current == ScreenId::Second);
    }
    bool isFinalScreen() const {
        return (current == ScreenId::Final);
    }

    // Used by Game before moving a player to this point
    bool isFreeCellForPlayer(const Point& p) const;

    // ---- helpers ----
    // Remove a door (digit '1'..'9') from this position (used when player opens it)
    void makePassage(const Point& p) {
        setCharAt(p, EMPTY_SPACE);
    }
    // Explicitly set the switch at p to ON or OFF (Game decides when to call).
    void setSwitchOn(const Point& p) {
        setCharAt(p, SWITCH_ON);
    }
    void setSwitchOff(const Point& p) {
        setCharAt(p, SWITCH_OFF);
    }
    // --- Switch logic API for Game ---
    void initFirstScreenSwitches();
    void initSecondScreenSwitches();
    void updateSwitchStates(const Player& p1, const Player& p2);
    void applySwitchEffect(const SwitchData& s, bool active);

	// Update gate states on first screen based on players' positions
    void updateFirstScreenGates(const Player& p1, const Player& p2);

    void triggerAutoBombs(const SwitchData& s);

    void placeBombAt(int x, int y);

    // (radius: Game decides value, e.g. from exercise spec)
    void clearExplosionArea(const Point& center, int radius);

    void collectPendingAutoBombs(std::vector<Point>& out);

    void printHint() const;

    void resetCurrent();

    void updateDoor7ByBinaryPuzzle();

};