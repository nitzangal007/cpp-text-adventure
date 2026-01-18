#include <cstring>
#include <iostream>
#include <fstream>

#include "Board.h"

// TODO: all magic chars such as 'W', 'X', '*' should be replaced with a constant
// (e.g. static constexpr char WALL = 'W';)
// and used in the code instead of the char itself
// (e.g. originalBoard[0][0] = WALL;)
// this way, if we decide to change the char, we will only need to change it in one place
// and not in the entire code

void Board::load(const std::string& filename) {
	std::ifstream screen_file(filename);
	// std::cout << screen_file.is_open() << std::endl;
	// TODO: handle errors (all sort of...) - do not submit it like that :)
	int curr_row = 0;
	int curr_col = 0;
	char c;
	while (!screen_file.get(c).eof() && curr_row < MAX_Y) {
		if (c == '\n') {
			if (curr_col < MAX_X) {
				// add spaces for missing cols
				#pragma warning(suppress : 4996) // to allow strcpy
				strcpy(originalBoard[curr_row] + curr_col, std::string(MAX_X - curr_col - 1, ' ').c_str());
			}
			++curr_row;
			curr_col = 0;
			continue;
		}
		if (curr_col < MAX_X) {
			// handle special chars
			if (c == '*') {
				startPos = { curr_col, curr_row, c };
			}
			else if (c == 'x' || c == 'X') {
				originalEndPos = { curr_col, curr_row, 'X' };
			}
			originalBoard[curr_row][curr_col++] = c;
		}
	}
	int last_row = (curr_row < MAX_Y ? curr_row : MAX_Y - 1);
	// add a closing frame
	// first line
	#pragma warning(suppress : 4996) // to allow strcpy
	strcpy(originalBoard[0], std::string(MAX_X, 'W').c_str());
	originalBoard[0][MAX_X] = '\n';
	// last line
	#pragma warning(suppress : 4996) // to allow strcpy
	strcpy(originalBoard[last_row], std::string(MAX_X, 'W').c_str());
	originalBoard[last_row][MAX_X] = '\0';
	// first col + last col
	for (int row = 1; row < last_row; ++row) {
		originalBoard[row][0] = 'W';
		originalBoard[row][MAX_X - 1] = 'W';
		originalBoard[row][MAX_X] = '\n';
	}
}

void Board::reset() {
	// copy the original board to the current board, as an entire 2d array
	memcpy(currentBoard, originalBoard, MAX_Y * (MAX_X + 1));
	player = startPos;
	player.setBoard(*this);
	endPos = originalEndPos;
}

void Board::print() const {
	if (!isSilent) {
		system("cls");
		// below single cout<< call prints the entire board,
		// as we have \n at the end of each line
		// (last line has \0 instead of \n)
		std::cout << currentBoard[0];
	}
}

void Board::randomlyChangeEndPoint() {
	Point newEndPos{0, 0};
	do {
		newEndPos = { rand() % MAX_X, rand() % MAX_Y, 'X'};
	} while (getChar(newEndPos) != ' ');
	if (!isSilent) {
		for (int i = 0; i < 5; i++) {
			// animate end point change (TODO: move to helper function)
			endPos.draw();
			newEndPos.erase();
			Sleep(10);
			endPos.erase();
			newEndPos.draw();
			Sleep(10);
		}
	}
	endPos = newEndPos;
}
