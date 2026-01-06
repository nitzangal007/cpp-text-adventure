#include "Switch.h"

std::vector<Point> Switch::getAndClearPendingTriggers()
{
    std::vector<Point> result = std::move(pendingTriggers_);
    pendingTriggers_.clear();
    return result;
}
