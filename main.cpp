#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#include "resource.h"
#include "AI.h"

// 以下为窗口程序
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

HINSTANCE Hinstance;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow) {
	static TCHAR szAppName[] = TEXT ("五子棋AI");
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON1) );
	wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE (IDR_MENU1);
	wndclass.lpszClassName = TEXT ("五子棋AI");

	if (!RegisterClass (&wndclass) ) {
		MessageBox (NULL, TEXT ("Failed to Register Class!"),
					szAppName, MB_ICONERROR);
		return 0;
	}

	Hinstance = hInstance;
	hwnd = CreateWindow (szAppName, TEXT ("五子棋AI"),
						 WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT, CW_USEDEFAULT,
						 610, 660,
						 NULL, NULL, hInstance, NULL);
	ShowWindow (hwnd, iCmdShow);
	UpdateWindow (hwnd);

	while (GetMessage (&msg, NULL, 0, 0) ) {
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	return msg.wParam;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HBITMAP hBitmap, white_hbitmap, black_hbitmap, select_hbitmap;
	static BITMAP bitmap, white_bitmap, black_bitmap, select_bitmap;
	static HINSTANCE hInstance;
	PAINTSTRUCT ps;
	HDC hdc, hdcMem;
	HMENU hMenu = ::GetMenu (hwnd);
	int wmId = LOWORD (wParam);
	// int wmEvent = HIWORD(wParam);
	// TCHAR str[ 100 ];
	// 定义AI类的实例ai
	static AI ai;
	static DWORD Difficult, Turn;
	int cxSource, cySource;
	static int cx, cy, px, py;
	static int color;
	// static HDC select_hdc;
	static int chessBoard[15][15]; // 棋盘

	switch (message) {
		case WM_CREATE :
			hInstance = ( (LPCREATESTRUCT) lParam)->hInstance;
			hBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP1) );
			GetObject (hBitmap, sizeof (BITMAP), &bitmap);
			white_hbitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP2) );
			GetObject (white_hbitmap, sizeof (BITMAP), &white_bitmap);
			black_hbitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP3) );
			GetObject (black_hbitmap, sizeof (BITMAP), &black_bitmap);
			select_hbitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (IDB_BITMAP4) );
			GetObject (select_hbitmap, sizeof (BITMAP), &select_bitmap);
			CheckMenuRadioItem (hMenu, ID_32774, ID_32774, ID_32774, MF_BYCOMMAND);
			Difficult = ID_32774;
			CheckMenuRadioItem (hMenu, ID_32778, ID_32778, ID_32778, MF_BYCOMMAND);
			Turn = ID_32778;
			// select_hdc = GetDC (hwnd);
			memset (chessBoard, -1, sizeof (chessBoard) );
			color = WHITE;// 一开始为玩家先手，所以计算机为白棋，color代表计算机所代表的棋的颜色
			return 0;

		case WM_COMMAND :
			switch (wmId) {
				case ID_32771 :
					MessageBoxA (hwnd, TEXT ("人工智能作业\n五子棋AI\n学号:10165102154\n姓名:张臻炜\n"), TEXT ("关于五子棋AI"),
								 MB_ICONINFORMATION);
					break;

				case ID_32772 :
goto_Start:
					memset (ai.chessBoard, NONE, sizeof (ai.chessBoard) );

					//设定AI程序的难度,需要在AI类中定义一个设定难度的函数，比如可以设定搜索的深度
					if (Difficult == ID_32774) {
						//设为简单
						ai.setDep (1);

					} else if (Difficult == ID_32775) {
						//设为中等
						ai.setDep (2);

					} else {
						//设为困难
						ai.setDep (4);
					}

					//设定AI程序的先后手，规定执黑先行
					if (Turn == ID_32777) {
						color = BLACK;//黑棋
						//AI先手，此处AI要产生第一步着法
						Point p = ai.GetFirstStep();
						chessBoard[p.x][p.y] = color;
						InvalidateRect (hwnd, NULL, TRUE);

					} else {
						color = WHITE;//白棋
					}

					break;

				case ID_32773 :
					if (MessageBoxA (hwnd, TEXT ("你确定要重新开始吗？\n"), TEXT ("你确定要重新开始吗？"), MB_ICONQUESTION | MB_YESNO) ==
							IDYES) {
goto_Restart:
						memset (chessBoard, -1, sizeof (chessBoard) );
						memset (ai.chessBoard, NONE, sizeof (ai.chessBoard) );
						InvalidateRect (hwnd, NULL, TRUE);
						goto goto_Start;
					}

					break;

				case ID_32780 :
					if (MessageBoxA (hwnd, TEXT ("你确定要退出吗？\n"), TEXT ("你确定要退出吗？"), MB_ICONQUESTION | MB_YESNO) ==
							IDYES) {
						DestroyWindow (hwnd);
					}

					break;

				case ID_32777 :
				case ID_32778 :
					if (wmId != (int) Turn) {
						CheckMenuRadioItem (hMenu, ID_32777, ID_32778, wmId, MF_BYCOMMAND);
						Turn = wmId;
						MessageBoxA (hwnd, TEXT ("先后手更改成功\n游戏已重新开始\n"), TEXT ("先后手更改成功"), MB_ICONINFORMATION);
						InvalidateRect (hwnd, NULL, TRUE);
						goto goto_Restart;
					}

					break;

				case ID_32774 :
				case ID_32775 :
				case ID_32776 :
					switch (wmId) {
						case ID_32774:
							MessageBoxA (hwnd, TEXT ("简单难度，搜索深度为1\n"), TEXT ("难度说明"), MB_ICONINFORMATION);
							break;

						case ID_32775:
							MessageBoxA (hwnd, TEXT ("中等难度，搜索深度为2\n"), TEXT ("难度说明"), MB_ICONINFORMATION);
							break;

						case ID_32776:
							MessageBoxA (hwnd, TEXT ("困难难度，搜索深度为4\n慎选！\n困难难度较难下赢\n且后期搜索规模会急剧增大，可能会导致程序卡死\n"),
										 TEXT ("难度说明"), MB_ICONINFORMATION);
							break;
					}

					if (wmId != (int) Difficult) {
						CheckMenuRadioItem (hMenu, ID_32774, ID_32776, wmId, MF_BYCOMMAND);
						Difficult = wmId;
						MessageBoxA (hwnd, TEXT ("难度更改成功\n游戏已重新开始\n"), TEXT ("难度更改成功"), MB_ICONINFORMATION);
						InvalidateRect (hwnd, NULL, TRUE);
						goto goto_Restart;
					}

					break;
			}

			return 0;

		case WM_LBUTTONDOWN:
			py = cx;
			px = cy;
			InvalidateRect (hwnd, NULL, TRUE);
			return 0;

		case WM_MOUSEMOVE: {
			int tmpx = LOWORD (lParam);
			int tmpy = HIWORD (lParam);
			cx = (tmpx - 18) / 40;
			cy = (tmpy - 14) / 40;

			if (tmpx < cx * 40 + 18 - 20) {
				cx -= 1;
			}

			if (tmpx > cx * 40 + 18 + 20) {
				cx += 1;
			}

			if (tmpy < cy * 40 + 14 - 20) {
				cy -= 1;
			}

			if (tmpy > cy * 40 + 14 + 20) {
				cy += 1;
			}
		}

		return 0;

		case WM_LBUTTONUP:
			if (px >= 0 && px < 15 && py >= 0 && py < 15 && chessBoard[px][py] == NONE) {
				hdc = GetDC (hwnd);
				//将玩家的着法传递给AI，玩家的着法为(px, py)
				ai.setPos (px, py, color ^ 1);
				chessBoard[px][py] = color ^ 1;
				hdcMem = CreateCompatibleDC (hdc);

				for (int i = 0; i < 15; ++i)
					for (int j = 0; j < 15; ++j) {
						if (chessBoard[i][j] == WHITE) {
							SelectObject (hdcMem, white_hbitmap);
							cxSource = white_bitmap.bmWidth;
							cySource = white_bitmap.bmHeight;
							BitBlt (hdc, j * 40, i * 40, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);

						} else if (chessBoard[i][j] == BLACK) {
							SelectObject (hdcMem, black_hbitmap);
							cxSource = black_bitmap.bmWidth;
							cySource = black_bitmap.bmHeight;
							BitBlt (hdc, j * 40, i * 40, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);
						}
					}

				if (ai.isGameOver (px, py) ) { //用于判断下完这一步之后游戏是否结束
					MessageBoxA (hwnd, TEXT ("恭喜你！\n你打败了五子棋AI！\n"), TEXT ("游戏胜利"), MB_ICONINFORMATION);
					memset (chessBoard, -1, sizeof (chessBoard) );
					memset (ai.chessBoard, NONE, sizeof (ai.chessBoard) );
					InvalidateRect (hwnd, NULL, TRUE);
					//goto goto_Restart;

				} else {
					//AI 生成下一步着法，
					Point p = ai.getNextStep (color);
					chessBoard[p.x][p.y] = color;

					for (int i = 0; i < 15; ++i)
						for (int j = 0; j < 15; ++j) {
							if (chessBoard[i][j] == WHITE) {
								SelectObject (hdcMem, white_hbitmap);
								cxSource = white_bitmap.bmWidth;
								cySource = white_bitmap.bmHeight;
								BitBlt (hdc, j * 40, i * 40, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);

							} else if (chessBoard[i][j] == BLACK) {
								SelectObject (hdcMem, black_hbitmap);
								cxSource = black_bitmap.bmWidth;
								cySource = black_bitmap.bmHeight;
								BitBlt (hdc, j * 40, i * 40, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);
							}
						}

					if (ai.isGameOver (p.x, p.y) ) { //用于判断下完这一步之后游戏是否结束
						MessageBoxA (hwnd, TEXT ("很遗憾！\n你输给了五子棋AI！\n"), TEXT ("游戏失败"), MB_ICONINFORMATION);
						memset (chessBoard, -1, sizeof (chessBoard) );
						memset (ai.chessBoard, NONE, sizeof (ai.chessBoard) );
						InvalidateRect (hwnd, NULL, TRUE);
						//goto goto_Restart;
					}
				}

				DeleteDC (hdcMem);
				ReleaseDC (hwnd, hdc);
			}

			return 0;

		case WM_PAINT : {
			hdc = BeginPaint (hwnd, &ps);
			hdcMem = CreateCompatibleDC (hdc);
			SelectObject (hdcMem, hBitmap);
			cxSource = bitmap.bmWidth;
			cySource = bitmap.bmHeight;
			BitBlt (hdc, 0, 0, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);

			for (int i = 0; i < 15; ++i)
				for (int j = 0; j < 15; ++j) {
					if (chessBoard[i][j] == WHITE) {
						SelectObject (hdcMem, white_hbitmap);
						cxSource = white_bitmap.bmWidth;
						cySource = white_bitmap.bmHeight;
						BitBlt (hdc, j * 40, i * 40, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);

					} else if (chessBoard[i][j] == BLACK) {
						SelectObject (hdcMem, black_hbitmap);
						cxSource = black_bitmap.bmWidth;
						cySource = black_bitmap.bmHeight;
						BitBlt (hdc, j * 40, i * 40, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);
					}
				}

			DeleteDC (hdcMem);
			EndPaint (hwnd, &ps);
			return 0;
		}

		case WM_DESTROY :
			PostQuitMessage (0);
			return 0;
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}
