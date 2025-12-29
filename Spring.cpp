#include "Spring.h"


void Spring::addSegment(const Point& p) {
    cells.push_back(p);
}

bool Spring::contains(const Point& p) const {
    for (const auto& cell : cells) {
        if (cell == p) return true;
    }
    return false;
}

bool Spring::canCompress(Direction moveDir) const {
    return moveDir == pushDir;
}

SpringLaunchParams Spring::calculateLaunchParams(int compressedCount) const {
    SpringLaunchParams params;
    params.speed = compressedCount;
    params.durationTicks = compressedCount * compressedCount;
    return params;
}