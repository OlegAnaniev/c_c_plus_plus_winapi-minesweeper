#include "Game.h"

Game::Game(int x, int y)
{
	isOver = 0;
	timeElapsed = 0;
	rows = y;
	cells = new Cell*[y];
	timer = 0;

	for (int i = 0; i < y; i++)
	{
		cells[i] = new Cell[x];
	}
}

Game::~Game()
{
	for (int i = 0; i < rows; i++)
	{
		delete[] cells[i];
	}

	delete[] cells;	
}