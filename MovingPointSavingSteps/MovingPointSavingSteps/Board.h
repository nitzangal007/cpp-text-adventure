#pragma once

#include <string>

#include "Point.h"

class Board {
	static constexpr int MAX_X = 80;
	static constexpr int MAX_Y = 25;
	char originalBoard[MAX_Y][MAX_X + 1];
	char currentBoard[MAX_Y][MAX_X + 1];
	Point startPos{ 0, 0 };
	Point player{ 0, 0 };
	Point originalEndPos{ 0, 0 };
	Point endPos{ 0, 0 };
	bool isSilent = false;
public:
	void load(const std::string& filename);
	void setIsSilent(bool isSilentMode) {
		isSilent = isSilentMode;
	}
	void reset();
	void print() const;
	char getChar(int x, int y) const {
		return currentBoard[y][x];
	}
	char getChar(Point p) const {
		return getChar(p.x, p.y);
	}
	bool finished() const {
		return player == endPos;
	}
	bool keyPressed(char key) {
		return player.keyPressed(key);
	}
	void erasePlayer() const {
		player.erase();
	}
	void drawPlayer() const {
		if(!isSilent) player.draw();
	}
	bool movePlayer() {
		return player.move();
	}
	void randomlyChangeEndPoint();
};

