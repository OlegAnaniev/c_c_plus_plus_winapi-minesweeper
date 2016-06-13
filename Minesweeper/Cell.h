#pragma once

struct Cell
{
	bool isBombed = 0;
	/*
	0 - closed
	1 - flagged
	2 - questioned
	3 - open
	*/
	int state = 0;
	int neighbourBombs = 0;
};