#pragma once

#include <iostream>

#include "utils.h"

class Board;

class Point {
	static constexpr char keys[] = { 'w', 'a', 'x', 'd', 's'};
	static constexpr size_t numKeys = sizeof(keys) / sizeof(keys[0]);
	struct Direction { int x, y; }; // inner private struct
	// the directions array order is exactly the same as the keys array - must keep it that way
	static constexpr Direction directions[] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0}, {0, 0} };
	int x, y;
	char ch;
	Direction dir{0, 0}; // current direction: dir.x, dir.y
	Board* pBoard = nullptr;
	void draw(char c) const {
		gotoxy(x, y);
		std::cout << c;
	}
	friend class Board;
public:
	Point(int x, int y) : Point(x, y, '*') {}
	Point(int x, int y, char c) : x(x), y(y), ch(c) {}
	void draw() const {
		draw(ch);
	}
	void erase() const {
		draw(' ');
	}
	bool keyPressed(char key);
	bool move();
	void setBoard(Board& board) {
		pBoard = &board;
	}
	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}
};

