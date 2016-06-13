#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <wchar.h>
#include <CommCtrl.h>

#include "Defines.h"
#include "Settings.h"
#include "Cell.h"
#include "Game.h"
#include "resource.h"

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcModal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcModal2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcModal3(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void loadSettings();
void loadBitmaps();
void deleteField();
void drawField();
Game* newGame(int x, int y, int firstX, int firstY);

Settings settings;
Game* game;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
#pragma region WindowClass
	WNDCLASSEX wcl;

	wcl.cbSize = sizeof(wcl);
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.lpfnWndProc = WinProc;
	wcl.hInstance = hInstance;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);// 16;
	wcl.lpszMenuName = 0;
	wcl.lpszClassName = L"MainWindowClass";

	if (!RegisterClassEx(&wcl))
		return 0;
#pragma endregion
	
#pragma region Window

	loadSettings();
	settings.hInstance = hInstance;

	HWND hWnd = CreateWindowEx(0, wcl.lpszClassName, L"Mines", WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU, 
		0, 0, 0, 150, 0, 0, hInstance, 0);	

	HWND hTimer1 = CreateWindowEx(WS_EX_LEFT, L"Static", 0, WS_VISIBLE | WS_CHILD | SS_BITMAP, 0, 0, 100, 50, hWnd,
		(HMENU)ID_TIMER1, hInstance, 0);
	HWND hTimer2 = CreateWindowEx(WS_EX_LEFT, L"Static", 0, WS_VISIBLE | WS_CHILD | SS_BITMAP, 26, 0, 100, 50, hWnd,
		(HMENU)ID_TIMER2, hInstance, 0);
	HWND hTimer3 = CreateWindowEx(WS_EX_LEFT, L"Static", 0, WS_VISIBLE | WS_CHILD | SS_BITMAP, 52, 0, 100, 50, hWnd,
		(HMENU)ID_TIMER3, hInstance, 0);
	SendMessage(hTimer1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bTimer[0]);
	SendMessage(hTimer2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bTimer[0]);
	SendMessage(hTimer3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bTimer[0]);

	HMENU hMenuMain = CreateMenu();	
	HMENU hMenuFile = CreatePopupMenu();	
	HMENU hMenuNew = CreatePopupMenu();

	AppendMenu(hMenuMain, MF_STRING | MF_POPUP, (UINT)hMenuFile, TEXT("&File"));
	
	AppendMenu(hMenuFile, MF_STRING | MF_POPUP, (UINT)hMenuNew, TEXT("&New"));
	AppendMenu(hMenuNew, MF_STRING, ID_BEGINNER, L"&Beginer");
	AppendMenu(hMenuNew, MF_STRING, ID_INTERMEDIATE, L"&Intermediate");
	AppendMenu(hMenuNew, MF_STRING, ID_EXPERT, L"&Expert");
	AppendMenu(hMenuNew, MF_STRING, ID_CUSTOM, L"&Custom...");
	
	AppendMenu(hMenuFile, MF_STRING, ID_HALLOFFAME, L"&Hall of fame");
	AppendMenu(hMenuFile, MF_STRING, ID_SETTINGS, L"&Settings");
	AppendMenu(hMenuFile, MF_SEPARATOR, 0, 0);
	AppendMenu(hMenuFile, MF_STRING, ID_ABOUT, L"&About");
	AppendMenu(hMenuFile, MF_STRING, ID_QUIT, L"&Quit");
		
	SetMenu(hWnd, hMenuMain);

#pragma endregion

#pragma region Other
	ShowWindow(settings.hWnd, cmdShow);
	UpdateWindow(settings.hWnd);
	drawField();

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#pragma endregion
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
		
	switch (message)
	{
	case WM_TIMER:
	
		switch (wParam)
		{
		case ID_TIMER_EVENT:

			if (game != NULL && game->timer < 1000)
			{
				int test;
				game->timer++;
				test = game->timer - game->timer % 100;
				SendDlgItemMessage(hWnd, ID_TIMER1, STM_SETIMAGE, IMAGE_BITMAP, 
					(LPARAM)settings.bTimer[game->timer / 100]);
				test = game->timer % 100 - game->timer % 10;
				SendDlgItemMessage(hWnd, ID_TIMER2, STM_SETIMAGE, IMAGE_BITMAP, 
					(LPARAM)settings.bTimer[game->timer % 100 / 10]);
				test = game->timer % 10;
				SendDlgItemMessage(hWnd, ID_TIMER3, STM_SETIMAGE, IMAGE_BITMAP, 
					(LPARAM)settings.bTimer[game->timer % 10]);
			}

			break;
		default:
			break;
		}
	
		break;
	case WM_COMMAND:
			
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			
			switch (LOWORD(wParam))
			{
			case ID_BEGINNER:
			
				deleteField();
				settings.cellsPerRow = 9;
				settings.cellsPerCol = 9;
				settings.bombAmount = 10;
				drawField();

				break;
			case ID_INTERMEDIATE:

				deleteField();
				settings.cellsPerRow = 16;
				settings.cellsPerCol = 16;
				settings.bombAmount = 40;
				drawField();

				break;
			case ID_EXPERT:

				deleteField();
				settings.cellsPerRow = 30;
				settings.cellsPerCol = 16;
				settings.bombAmount = 99;
				drawField();

				break;
			case ID_CUSTOM:
				deleteField();
				DialogBox(settings.hInstance, (LPCWSTR)IDD_DIALOG1, hWnd, DlgProcModal);
				drawField();
				break;
			case ID_HALLOFFAME:
				break;
			case ID_SETTINGS:
				DialogBox(settings.hInstance, (LPCWSTR)IDD_DIALOG2, hWnd, DlgProcModal2);
				break;
			case ID_ABOUT:
				DialogBox(settings.hInstance, (LPCWSTR)IDD_DIALOG3, hWnd, DlgProcModal3);
				break;
			case ID_QUIT:
				PostQuitMessage(0);
				break;
			default:
				
				int tempY = LOWORD(wParam) / 100; 
				int tempX = LOWORD(wParam) % 100;

				if (game == NULL)
				{
					game = newGame(settings.cellsPerRow, settings.cellsPerCol, tempX, tempY);
				}
												
				if (game->cells[tempY][tempX].state == 1 || game->cells[tempY][tempX].state == 3)
				{
					break;
				}
				
				game->cells[tempY][tempX].state = 3;

				if (game->cells[tempY][tempX].isBombed)
				{
					SendMessage(GetDlgItem(hWnd, LOWORD(wParam)), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bBomb);
				}
				else
				{
					if (game->cells[tempY][tempX].neighbourBombs == 0)
					{
						if (!(tempY == 0 || tempX == 0))
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY - 1) * 100 + (tempX - 1), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}

						if (tempY != 0)
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY - 1) * 100 + (tempX), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}

						if (!(tempY == 0 || tempX == settings.cellsPerRow - 1))
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY - 1) * 100 + (tempX + 1), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}

						if (tempX != settings.cellsPerRow - 1)
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY)* 100 + (tempX + 1), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}


						if (!(tempY == settings.cellsPerCol - 1 || tempX == settings.cellsPerRow - 1))
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY + 1) * 100 + (tempX + 1), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}

						if (tempY != settings.cellsPerCol - 1)
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY + 1) * 100 + (tempX), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}

						if (!(tempY == settings.cellsPerCol - 1 || tempX == 0))
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY + 1) * 100 + (tempX - 1), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}

						if (tempX != 0)
						{
							SendMessage(settings.hWnd, WM_COMMAND, MAKEWPARAM((tempY)* 100 + (tempX - 1), BN_CLICKED),
								(LPARAM)settings.hWnd);
						}
					}

					SendMessage(GetDlgItem(hWnd, LOWORD(wParam)), BM_SETIMAGE, IMAGE_BITMAP,
						(LPARAM)settings.bNums[game->cells[tempY][tempX].neighbourBombs]);
				}
										
				SetWindowLongPtr(GetDlgItem(hWnd, LOWORD(wParam)), GWL_STYLE,
					GetWindowLongPtr(GetDlgItem(hWnd, LOWORD(wParam)), GWL_STYLE) | BS_FLAT);
				break;
			}
			
			RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
		
			break;
		default:
			break;
		}

		break;
	
	case WM_CONTEXTMENU:
	{		
		if (game == NULL)
		{
			break;
		}
						   
		RECT rect;
		RECT rect_cl;
		GetWindowRect(settings.hWnd, &rect);
		GetClientRect(settings.hWnd, &rect_cl);

		int tempX = (HIWORD(lParam) - 50 - rect.top - (rect.bottom - rect_cl.bottom - rect.top)) / settings.cellHeight;
		int tempY = (LOWORD(lParam) - rect.left - (rect.right - rect_cl.right - rect.left)) / settings.cellWidth;
		int id = tempX * 100 + tempY;

		int ppcpot = game->cells[tempX][tempY].state;

		switch (game->cells[tempX][tempY].state)
		{
		case 0:
			game->cells[tempX][tempY].state = 1;
			SendMessage(GetDlgItem(hWnd, id), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bFlag);
			break;
		case 1:
			game->cells[tempX][tempY].state = 2;
			SendMessage(GetDlgItem(hWnd, id), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bQuest);
			break;
		case 2:
			game->cells[tempX][tempY].state = 0;
			SendMessage(GetDlgItem(hWnd, id), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bClosed);
			break;
		default:
			break;
		}		
	}		
		break;
	case WM_CREATE:
		SetTimer(hWnd, ID_TIMER_EVENT, 1000, 0);
		settings.hWnd = hWnd;
		

		break;
	case WM_DESTROY:
		delete game;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


BOOL CALLBACK DlgProcModal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, NULL);
		break;

	case WM_INITDIALOG:

		WCHAR newValue[10];
		

		SendDlgItemMessage(hDlg, IDC_SLIDER_HEIGHT, TBM_SETRANGE, 0, MAKELPARAM(5, 40));
		SendDlgItemMessage(hDlg, IDC_SLIDER_HEIGHT, TBM_SETTICFREQ, 1, 0);
		SendDlgItemMessage(hDlg, IDC_SLIDER_HEIGHT, TBM_SETPOS, TRUE, settings.cellsPerCol);

		SendDlgItemMessage(hDlg, IDC_SLIDER_WIDTH, TBM_SETRANGE, 0, MAKELPARAM(5, 40));
		SendDlgItemMessage(hDlg, IDC_SLIDER_WIDTH, TBM_SETTICFREQ, 1, 0);
		SendDlgItemMessage(hDlg, IDC_SLIDER_WIDTH, TBM_SETPOS, TRUE, settings.cellsPerRow);

		SendDlgItemMessage(hDlg, IDC_SLIDER_BOMBS, TBM_SETRANGE, 0, MAKELPARAM(5, 100));
		SendDlgItemMessage(hDlg, IDC_SLIDER_BOMBS, TBM_SETTICFREQ, 1, 0);
		SendDlgItemMessage(hDlg, IDC_SLIDER_BOMBS, TBM_SETPOS, TRUE, settings.bombAmount);

		SetDlgItemText(hDlg, IDC_EDIT_HEIGHT, _itow(settings.cellsPerCol, newValue, 10));
		SetDlgItemText(hDlg, IDC_EDIT_WIDTH, _itow(settings.cellsPerRow, newValue, 10));
		SetDlgItemText(hDlg, IDC_EDIT_BOMBS, _itow(settings.bombAmount, newValue, 10));
		
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HEIGHT), false);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_WIDTH), false);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BOMBS), false);


		return TRUE;
#pragma region Button Events
	case WM_COMMAND:
	
		switch (LOWORD(wParam))
		{
		case IDOK:
			settings.cellsPerCol = SendDlgItemMessage(hDlg, IDC_SLIDER_WIDTH, TBM_GETPOS, 0, 0);
			settings.cellsPerRow = SendDlgItemMessage(hDlg, IDC_SLIDER_HEIGHT, TBM_GETPOS, 0, 0);
			settings.bombAmount = SendDlgItemMessage(hDlg, IDC_SLIDER_BOMBS, TBM_GETPOS, 0, 0);		
		case IDCANCEL:
			EndDialog(hDlg, NULL);
			break;
		default:
			break;
		}		
	
	break;
#pragma endregion
	case WM_HSCROLL:
	{
		WCHAR tempValue[4];
		int position;
		int id = GetDlgCtrlID((HWND)lParam);

		switch (id)
		{
		case IDC_SLIDER_WIDTH:
			position = SendDlgItemMessage(hDlg, IDC_SLIDER_WIDTH, TBM_GETPOS, 0, 0);
			_itow(position, tempValue, 10);
			SetDlgItemText(hDlg, IDC_EDIT_WIDTH, tempValue);
			break;
		case IDC_SLIDER_HEIGHT:
			position = SendDlgItemMessage(hDlg, IDC_SLIDER_HEIGHT, TBM_GETPOS, 0, 0);
			_itow(position, tempValue, 10);
			SetDlgItemText(hDlg, IDC_EDIT_HEIGHT, tempValue);
			break;
		case IDC_SLIDER_BOMBS:
			position = SendDlgItemMessage(hDlg, IDC_SLIDER_BOMBS, TBM_GETPOS, 0, 0);
			_itow(position, tempValue, 10);
			SetDlgItemText(hDlg, IDC_EDIT_BOMBS, tempValue);
			break;
		default:
			break;
		}		
	}
		break;
	}
	return FALSE;
}

BOOL CALLBACK DlgProcModal2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, NULL);
		break;

	case WM_INITDIALOG:

		WCHAR newValue[10];


		SendDlgItemMessage(hDlg, IDC_SLIDER_CELLWIDTH, TBM_SETRANGE, 0, MAKELPARAM(20, 50));
		SendDlgItemMessage(hDlg, IDC_SLIDER_CELLWIDTH, TBM_SETTICFREQ, 5, 0);
		SendDlgItemMessage(hDlg, IDC_SLIDER_CELLWIDTH, TBM_SETPOS, TRUE, settings.cellWidth);

		SendDlgItemMessage(hDlg, IDC_SLIDER_CELLHEIGHT, TBM_SETRANGE, 0, MAKELPARAM(20, 50));
		SendDlgItemMessage(hDlg, IDC_SLIDER_CELLHEIGHT, TBM_SETTICFREQ, 5, 0);
		SendDlgItemMessage(hDlg, IDC_SLIDER_CELLHEIGHT, TBM_SETPOS, TRUE, settings.cellHeight);

		SetDlgItemText(hDlg, IDC_EDIT_CELLWIDTH, _itow(settings.cellWidth, newValue, 10));
		SetDlgItemText(hDlg, IDC_EDIT_CELLHEIGHT, _itow(settings.cellHeight, newValue, 10));

		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CELLWIDTH), false);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CELLHEIGHT), false);	

		SendDlgItemMessage(hDlg, IDC_CHECK_SQUARE, BM_SETCHECK, BST_CHECKED, 0);

		return TRUE;
#pragma region Button Events
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:
			settings.cellWidth = SendDlgItemMessage(hDlg, IDC_SLIDER_CELLWIDTH, TBM_GETPOS, 0, 0);
			settings.cellHeight = SendDlgItemMessage(hDlg, IDC_SLIDER_CELLHEIGHT, TBM_GETPOS, 0, 0);
			settings.cellChanged = true;			
		case IDCANCEL:
			EndDialog(hDlg, NULL);
			break;
		default:
			break;
		}
		
		break;
#pragma endregion
	case WM_HSCROLL:
	{
					   WCHAR tempValue[4];
					   int position;
					   int id = GetDlgCtrlID((HWND)lParam);


					   switch (id)
					   {
					   case IDC_SLIDER_CELLWIDTH:
						   position = SendDlgItemMessage(hDlg, IDC_SLIDER_CELLWIDTH, TBM_GETPOS, 0, 0);
						   _itow(position, tempValue, 10);
						   SetDlgItemText(hDlg, IDC_EDIT_CELLWIDTH, tempValue);
						   if (SendDlgItemMessage(hDlg, IDC_CHECK_SQUARE, BM_GETSTATE,0,0) == BST_CHECKED)
						   {
							   SendDlgItemMessage(hDlg, IDC_SLIDER_CELLHEIGHT, TBM_SETPOS, TRUE, position);
							   SetDlgItemText(hDlg, IDC_EDIT_CELLHEIGHT, tempValue);
						   }

						   break;
					   case IDC_SLIDER_CELLHEIGHT:
						   position = SendDlgItemMessage(hDlg, IDC_SLIDER_CELLHEIGHT, TBM_GETPOS, 0, 0);
						   _itow(position, tempValue, 10);
						   SetDlgItemText(hDlg, IDC_EDIT_CELLHEIGHT, tempValue);

						   if (SendDlgItemMessage(hDlg, IDC_CHECK_SQUARE, BM_GETSTATE, 0, 0) == BST_CHECKED)
						   {
							   SendDlgItemMessage(hDlg, IDC_SLIDER_CELLWIDTH, TBM_SETPOS, TRUE, position);
							   SetDlgItemText(hDlg, IDC_EDIT_CELLWIDTH, tempValue);
						   }

						   break;
					   default:
						   break;
					   }


	}
		break;
	}
	return FALSE;
}

BOOL CALLBACK DlgProcModal3(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, NULL);
		break;
	default:
		break;
	}
	return 0;
}


void loadSettings()
{
	HBITMAP bitmap;

	bitmap = (HBITMAP)LoadImage(NULL, L"Timer0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
	settings.bTimer[0] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[1] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[2] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[3] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[4] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[5] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer6.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[6] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer7.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[7] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer8.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[8] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);
	bitmap = (HBITMAP)LoadImage(NULL, L"Timer9.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	settings.bTimer[9] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, 25, 50, 0);


	settings.cellHeight = 20;
	settings.cellWidth = 20;
	settings.cellsPerRow = 25;
	settings.cellsPerCol = 30;
	settings.bombAmount = 20;

	settings.cellChanged = false;
	
	loadBitmaps();
}

void loadBitmaps()
{
	HBITMAP bitmap;
	
	bitmap = (HBITMAP)LoadImage(NULL, L"Empty.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[0] != NULL)
	{
		DeleteObject(settings.bNums[0]);

	}
	settings.bNums[0] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[1] != NULL)
	{
		DeleteObject(settings.bNums[1]);

	}
	settings.bNums[1] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[2] != NULL)
	{
		DeleteObject(settings.bNums[2]);

	}
	settings.bNums[2] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[3] != NULL)
	{
		DeleteObject(settings.bNums[3]);

	}
	settings.bNums[3] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[4] != NULL)
	{
		DeleteObject(settings.bNums[4]);

	}
	settings.bNums[4] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[5] != NULL)
	{
		DeleteObject(settings.bNums[5]);

	}
	settings.bNums[5] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"6.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[6] != NULL)
	{
		DeleteObject(settings.bNums[6]);

	}
	settings.bNums[6] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"7.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[7] != NULL)
	{
		DeleteObject(settings.bNums[7]);

	}
	settings.bNums[7] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"8.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bNums[8] != NULL)
	{
		DeleteObject(settings.bNums[8]);

	}
	settings.bNums[8] = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"Closed.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bClosed != NULL)
	{
		DeleteObject(settings.bClosed);

	}
	settings.bClosed = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"Flag.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bFlag != NULL)
	{
		DeleteObject(settings.bFlag);

	}
	settings.bFlag = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"Quest.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bQuest != NULL)
	{
		DeleteObject(settings.bQuest);

	}
	settings.bQuest = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);

	bitmap = (HBITMAP)LoadImage(NULL, L"Bomb.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (settings.bBomb != NULL)
	{
		DeleteObject(settings.bBomb);

	}
	settings.bBomb = (HBITMAP)CopyImage((HWND)bitmap, IMAGE_BITMAP, settings.cellWidth, settings.cellHeight, 0);
}

void deleteField()
{
	for (int i = 0; i < settings.cellsPerCol; i++)
	{
		for (int j = 0; j < settings.cellsPerRow; j++)
		{	
			DestroyWindow(GetDlgItem(settings.hWnd, i * 100 + j));
		}
	}

	delete game;
	game = NULL;
}


void drawField()
{
	RECT rect;
	RECT rect_cl;
	GetWindowRect(settings.hWnd, &rect);
	GetClientRect(settings.hWnd, &rect_cl);
	int x_offset = rect.right - rect_cl.right - rect.left;
	int y_offset = rect.bottom - rect_cl.bottom - rect.top + 50;


	if (settings.cellChanged)
	{
		loadBitmaps();
		settings.cellChanged = false;
	}
		
	SetWindowPos(settings.hWnd, HWND_TOP, 0, 0, 
		settings.cellWidth * settings.cellsPerRow + x_offset, settings.cellHeight * settings.cellsPerCol + y_offset, NULL);
		
	int id;

	for (int i = 0; i < settings.cellsPerCol; i++)
	{
		for (int j = 0; j < settings.cellsPerRow; j++)
		{
			id = (i * 100 + j);
			CreateWindowEx(0, L"Button", L"*", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_BITMAP,
				settings.cellWidth * j, 50 + settings.cellHeight * i, settings.cellWidth, settings.cellHeight, settings.hWnd,
				(HMENU)id, settings.hInstance, 0);

			SendMessage(GetDlgItem(settings.hWnd, id), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)settings.bClosed);

		}
	}	
}

Game* newGame(int x, int y, int firstX, int firstY)
{
	Game* temp = new Game(x, y);

	int bombsToPlace = settings.bombAmount;
	int tempX;
	int tempY;

	srand(time(NULL));

	while (bombsToPlace)
	{
		tempX = rand() % settings.cellsPerRow;
		tempY = rand() % settings.cellsPerCol;
		if (temp->cells[tempY][tempX].isBombed == 0 && (tempX != firstX || tempY != firstY))
		{
			temp->cells[tempY][tempX].isBombed = 1;
			bombsToPlace--;
		}
	}

	for (int i = 0; i < settings.cellsPerRow; i++)
	{
		for (int j = 0; j < settings.cellsPerCol; j++)
		{
			if (!(j == 0 || i == 0))
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j - 1][i - 1].isBombed;
			}
			if (j != 0)
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j-1][i].isBombed;
			}
			if (!(i == settings.cellsPerRow - 1 || j==0))
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j - 1][i + 1].isBombed;
			}
			if (i != settings.cellsPerRow - 1)
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j][i+1].isBombed;
			}
			if (!(i == settings.cellsPerRow - 1 || j == settings.cellsPerCol - 1))
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j + 1][i + 1].isBombed;
			}
			if (j != settings.cellsPerCol - 1)
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j+1][i].isBombed;
			}
			if (!(i == 0 || j == settings.cellsPerCol - 1))
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j + 1][i - 1].isBombed;
			}
			if (i != 0)
			{
				temp->cells[j][i].neighbourBombs += temp->cells[j][i-1].isBombed;
			}
		}
	}

	return temp;

}