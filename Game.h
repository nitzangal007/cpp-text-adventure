#pragma once
#include <conio.h>
#include "Player.h"
#include "Screens.h"
#include "Menu.h"



enum class GameResult { BackToMenu, QuitProgram };




class Game
{
	struct Bomb {								
		bool active = false;
		Point pos;
		int  ticksLeft = 0;                      // time left until explosion
	};

	Screens currentScreen;
	Player player1;
	Player player2;
	Point   player1Start;
	Point   player2Start;
	Bomb bomb;


	

public:
	Game() :player1(Player::Id::First, Point(5, 5, 0, 0, '$'), "wdxas", '$'),
		player2(Player::Id::Second, Point(10, 10, 0, 0, '&'), "ilmjk", '&'),
		player1Start(5, 5, 0, 0, '$'),
		player2Start(10, 10, 0, 0, '&')
	{
	}

	void initGame();
	void run();
	void runGame();
	void updatePlayerMovement(Player& player);
	void updateSwitchRows();
	void collectItemIfPossible(Player& player);
	void drawStatusBar();
	void tryPlaceBomb(Player& player);
	void explodeBomb();
	bool isPlayerInExplosion(const Player& player, const Point& center, int radiusSquared) ;
		






};
