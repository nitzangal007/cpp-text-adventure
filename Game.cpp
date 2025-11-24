#include "Menu.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include "utils.h"
#include "Player.h"

enum Keys { ESC = 27 };

int main() {
	hideCursor();
	Menu menu;
	Player players[] = {
		Player(Player::Id::First, Point(5, 5, 0, 0, '$'), "wdxas", '$'),
		Player(Player::Id::Second, Point(10, 10, 0, 0, '&'), "ilmjk", '&')
	};
	Options playerChoice = menu.runOnce();
	cls();
	if (playerChoice == EXIT_GAME) {
		return 0;
	}
	for (auto& player : players) {
		player.draw();
	}
	while (true) {
		if (_kbhit()) {
			char ch = _getch();
			if (ch == ESC) {
				break;
			}
			for (auto& player : players) {
				player.handleKeyPress(ch);
			}
		}
		for (auto& player : players) {
			player.move();
			player.draw();
		}
		Sleep(100);
	}
	return 0;
}