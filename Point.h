#pragma once												// Point.h
#include <iostream>											// We took the code from class
#include "utils.h"
#include "Direction.h"
#include "ColorUtils.h"


class Point {
	int x = 1, y = 1;
	int diff_x = 1, diff_y = 0;
	char ch = '*';
public:
	Point() {}
	Point(int x1, int y1)
		: x(x1), y(y1), diff_x(0), diff_y(0), ch(' ') {
	}
	Point(int x1, int y1, int diffx, int diffy, char c) : x(x1), y(y1), diff_x(diffx), diff_y(diffy), ch(c) {}
	void draw() {
		draw(ch);
	}
	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}
	bool operator!=(const Point& other) const {
		return !(*this == other);
	}
	void draw(char c) {
		gotoxy(x, y);
		if (g_colorsEnabled)
			setConsoleColor(getColorForChar(c));
		std::cout << c;
		if (g_colorsEnabled)
			resetColor();
	}
	void move();
	void setDirection(Direction dir);
	void setPosition(int x1, int y1) {
		x = x1;
		y = y1;
	}
	int getX() const {
		return x;
	}
	int getY() const {
		return y;
	}
	int getDiffX() const { return diff_x; }
	int getDiffY() const { return diff_y; }
	Direction getDirection() const {
		if (diff_x == 0 && diff_y == -1)
			return Direction::UP;
		else if (diff_x == 1 && diff_y == 0)
			return Direction::RIGHT;
		else if (diff_x == 0 && diff_y == 1)
			return Direction::DOWN;
		else if (diff_x == -1 && diff_y == 0)
			return Direction::LEFT;
		else
			return Direction::STAY;
	}
};

