#include "Spring.h"

Spring::Spring(int id, const Point& anchor, Direction pushDir, Direction releaseDir)
    : id(id), anchorWall(anchor), pushDir(pushDir), releaseDir(releaseDir), maxLength(0)
{
}

void Spring::addSegment(const Point& p) {
    cells.push_back(p);
    maxLength++;
}

bool Spring::contains(const Point& p) const {
    for (const auto& cell : cells) {
        if (cell == p) return true;
    }
    return false;
}

bool Spring::canCompress(Direction moveDir) const {
    // Player must move in the direction of the wall (pushDir)
    return moveDir == pushDir;
}

int Spring::calculateCompressedCount(const Point& playerPos) const {
    // Logic to determine how deep the player is:
    // Iterate cells and find distance/index relative to the start/end
    // (Skeleton only - needs implementation)
    return 0;
}

