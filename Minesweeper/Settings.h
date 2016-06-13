struct Settings
{
	int cellWidth;
	int cellHeight;
	bool cellChanged;

	int cellsPerRow;
	int cellsPerCol;

	int bombAmount;

	HBITMAP bClosed;
	HBITMAP bFlag;
	HBITMAP bQuest;
	HBITMAP bBomb;

	HBITMAP bNums[9];
	HBITMAP bTimer[10];

	HINSTANCE hInstance;
	HWND hWnd;
	HMENU hMenu;

};