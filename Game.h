#pragma once
#include <conio.h>
#include "Player.h"
#include "Screens.h"



enum class GameResult { BackToMenu, QuitProgram };




class Game
{
	Screens currentScreen;
	Player player1;
	Player player2;


public:
	Game() :player1(Player::Id::First, Point(5, 5, 0, 0, '$'), "wdxas", '$'),
		player2(Player::Id::Second, Point(10, 10, 0, 0, '&'), "ilmjk", '&')
	{
	}
	void updatePlayerMovement(Player& player);
	void updateSwitchRows();
	void collectItemIfPossible(Player& player);
	void drawStatusBar();
	void initGame();
	void runGame();






};
