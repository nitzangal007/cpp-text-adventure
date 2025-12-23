#pragma once
#include <vector>
#include "Point.h"
#include "Direction.h"
//#include "GameConstants.h" // Uncomment if you have this file

class Spring {
private:
    int id;                      // Unique ID
    std::vector<Point> cells;    // Coordinates of the spring segments ('#')
    Point anchorWall;            // Position of the supporting wall
    Direction pushDir;           // Direction required to compress
    Direction releaseDir;        // Direction of launch
    int maxLength;               // Total length

public:
    // Constructor
    Spring(int id, const Point& anchor, Direction pushDir, Direction releaseDir);

    // -- Setters / Builders --
    void addSegment(const Point& p);

    // -- Getters --
    int getId() const { return id; }
    Direction getReleaseDirection() const { return releaseDir; }
    Direction getPushDirection() const { return pushDir; }
    const std::vector<Point>& getCells() const { return cells; }
    size_t getLength() const { return cells.size(); }

    // -- Logic --

    // Checks if the given point is part of this spring
    bool contains(const Point& p) const;

    // Checks if a player moving in 'moveDir' can enter/compress this spring
    bool canCompress(Direction moveDir) const;

    // Calculates how many segments are compressed based on player position
    // (Returns 0 if not on spring)
    int calculateCompressedCount(const Point& playerPos) const;
};