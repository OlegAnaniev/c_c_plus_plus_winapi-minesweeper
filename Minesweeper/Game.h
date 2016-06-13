#pragma once
#include "Cell.h"

struct Game
{
	Cell** cells;
	bool isOver;
	int timeElapsed;
	int rows;
	int timer;

	Game(int x, int y);
	~Game();
};