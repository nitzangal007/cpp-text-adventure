#pragma once
#include "Point.h"
#include <string>
#include <array>
#include <cctype>

class Riddle
{
    Point position;
    std::string question;
    std::array<std::string, 4> options; 
    int correctIndex_ = 0;              
    bool solved_ = false;
    int wrongAttempts_ = 0;

public:
	Riddle() = default;
    Riddle(const Point& pos, std::string q, const std::array<std::string, 4>& opts, int correctIndex);


    const Point& getPosition() const { return position; }
    const std::string& getQuestion() const { return question; }
    const std::array<std::string, 4>& getOptions() const { return options; }

    bool isSolved() const { return solved_; }
    int wrongAttempts() const { return wrongAttempts_; }

    // choice: 0..3
    bool trySolve(int choiceIndex);

    // Helper: convert input like 'A'/'b'/'1'/'4' -> 0..3 or -1 invalid
    static int parseChoice(const std::string& s);
};







