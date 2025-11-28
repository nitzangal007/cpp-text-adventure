#pragma once
#include <conio.h>
#include "Player.h"
#include "Point.h"



enum class GameResult { BackToMenu, QuitProgram };




class Game
{
	Player player1;
	Player player2;


public:
	Game() :player1(Player::Id::First, Point(5, 5, 0, 0, '$'), "wdxas", '$'),
		player2(Player::Id::Second, Point(10, 10, 0, 0, '&'), "ilmjk", '&')
	{
	}
	void initGame();
	void runGame();






};
