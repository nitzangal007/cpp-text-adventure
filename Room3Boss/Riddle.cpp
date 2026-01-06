#include "Riddle.h"



Riddle::Riddle(const Point& pos,
    std::string q,
    const std::array<std::string, 4>& opts,
    int correctIndex)
    : position(pos), question(std::move(q)), options(opts), correctIndex_(correctIndex)
{
    if (correctIndex_ < 0) correctIndex_ = 0;
    if (correctIndex_ > 3) correctIndex_ = 3;

}


bool Riddle::trySolve(int choiceIndex)
{
    if (solved_)
        return true;

    if (choiceIndex == correctIndex_)
    {
        solved_ = true;
        return true;
    }

    wrongAttempts_++;
    return false;
}


int Riddle::parseChoice(const std::string& s)
{
    size_t i = 0;
    while (i < s.size() && std::isspace((unsigned char)s[i])) i++;
    if (i >= s.size()) return -1;

    char c = (char)std::toupper((unsigned char)s[i]);

    if (c >= '1' && c <= '4')
        return (c - '1');

    if (c >= 'A' && c <= 'D')
        return (c - 'A');

    return -1;
}
