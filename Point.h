#pragma once

#include <iostream>
#include "utils.h"
#include "Direction.h"



class Point {
	int x = 1, y = 1;
	int diff_x = 1, diff_y = 0;
	char ch = '*';
public:
	Point() {}
	Point(int x1, int y1, int diffx, int diffy, char c) {
		x = x1;
		y = y1;
		diff_x = diffx;
		diff_y = diffy;
		ch = c;
	}
	void draw() {
		draw(ch);
	}
	void draw(char c) {
		gotoxy(x, y);
		std::cout << c;
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
};

