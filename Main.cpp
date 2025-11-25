#include "Menu.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include "utils.h"
#include "Player.h"
#include "Game.h"


int main() {
	hideCursor();
	Menu menu;


	while (TRUE) {
		cls();
		Options PlayerChoice = menu.runOnce();
		if(PlayerChoice == EXIT_GAME) {
			break;
		}
		else {
			Game game;
			cls();
			game.initGame();
			game.runGame();
		}
		
		
	}

}
	