#include"Method.h"

extern bool isstart;
extern HWND hwndButton1;
extern HWND hwndButton2;
extern HWND hwndButton3;
extern HWND hwndButton4;
extern HWND _hwnd;
extern HDC hdc;

void START(HWND hWnd) {}

void Get_Init_UI(HWND hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	hwndButton1 = CreateWindow(
		L"BUTTON",	// predefined class    不区分大小写
		L"暂停",       // button text
		WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

	   // Size and position values are given explicitly, because
	   // the CW_USEDEFAULT constant gives zero values for buttons.
		10,         // starting x position
		10,         // starting y position
		80,        // button width
		30,        // button height
		hWnd,       // parent window
		(HMENU)IDB_ONE,       // No menu
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL);      // pointer not needed
	ShowWindow(hwndButton1, SW_HIDE);
	hwndButton2 = CreateWindow(
		L"BUTTON",	// predefined class    不区分大小写
		L"开始游戏",       // button text
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

	   // Size and position values are given explicitly, because
	   // the CW_USEDEFAULT constant gives zero values for buttons.
		(rect.left+rect.right) / 2 - 90,         // starting x position
		120,         // starting y position
		180,        // button width
		50,        // button height
		hWnd,       // parent window
		(HMENU)IDB_TWO,       // No menu
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL);      // pointer not needed
	hwndButton3 = CreateWindow(
		L"BUTTON",	// predefined class    不区分大小写
		L"设置",       // button text
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

	   // Size and position values are given explicitly, because
	   // the CW_USEDEFAULT constant gives zero values for buttons.
		(rect.left+rect.right) / 2 - 90,         // starting x position
		220,         // starting y position
		180,        // button width
		50,        // button height
		hWnd,       // parent window
		(HMENU)IDB_THREE,       // No menu
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL);      // pointer not needed
	hwndButton4 = CreateWindow(
		L"BUTTON",	// predefined class    不区分大小写
		L"退出游戏",       // button text
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

	   // Size and position values are given explicitly, because
	   // the CW_USEDEFAULT constant gives zero values for buttons.
		(rect.left+rect.right) / 2 - 90,         // starting x position
		320,         // starting y position
		180,        // button width
		50,        // button height
		hWnd,       // parent window
		(HMENU)IDB_FOUR,       // No menu
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL);      // pointer not needed
}

void Return_To_Mune() {
	RECT rect;
	GetClientRect(_hwnd, &rect);
	hdc = GetDC(_hwnd);
	SelectObject(hdc, WHITE_BRUSH);
	Rectangle(hdc, -10, -10, rect.right, rect.bottom);
	InvalidateRect(_hwnd, NULL, TRUE);

	ShowWindow(hwndButton1, SW_HIDE);

	isstart = false;
	ShowWindow(hwndButton2, SW_SHOW);
	ShowWindow(hwndButton3, SW_SHOW);
	ShowWindow(hwndButton4, SW_SHOW);

}