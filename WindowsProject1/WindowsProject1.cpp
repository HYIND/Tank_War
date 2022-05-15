// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "WindowsProject1.h"
#include "Method.h"
#include <comdef.h>
#include "GdiPlus.h"
using namespace Gdiplus;

#define MAX_LOADSTRING 100

//extern vector <tank_info*> tank_list;

// 全局变量:
Tank tank1(80, 80);
Tank tank2(80, 80);
HDC hdcmem, hdc;
bool isstart = false;

//加载位图
HBITMAP hbitmap1 = (HBITMAP)LoadImage(NULL, L"beila2.bmp", IMAGE_BITMAP, tank1.width, tank1.height, LR_DEFAULTSIZE | LR_LOADFROMFILE);
HBITMAP hbitmap2 = (HBITMAP)LoadImage(NULL, L"jiaran.bmp", IMAGE_BITMAP, tank2.width, tank2.height, LR_DEFAULTSIZE | LR_LOADFROMFILE);
BITMAP bm1;
BITMAP bm2;
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING] = TEXT("Tank War");                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND _hwnd;
RECT _rect;

HWND hwndButton1;
HWND hwndButton2;
HWND hwndButton3;
HWND hwndButton4;

HPEN tank_Pen = CreatePen(PS_SOLID, 0, RGB(100, 100, 200));
HBRUSH tank_Brush = CreateSolidBrush(RGB(255, 238, 114));
HPEN bullet_Pen = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
HBRUSH bullet_Brush = CreateSolidBrush(RGB(255, 0, 0));


WSADATA wsa;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Pause(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	// TODO: 在此处放置代码。
	{
	}
	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));


	MSG msg;
	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
enum { tank, bullet };
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	GetClientRect(hWnd, &rect);
	_rect = rect;
	_hwnd = hWnd;
	switch (message)
	{
	case WM_CREATE:
	{
		tank1.InitTank(rect.right - tank1.width / 2 - 20, (rect.bottom - rect.top) / 2, LEFT);
		tank2.InitTank(rect.left + tank2.width / 2 + 20, (rect.bottom - rect.top) / 2, RIGHT);
		Get_Init_UI(_hwnd);
		Get_Initinfo();
		GetObject(hbitmap1, sizeof(BITMAP), &bm1);
		GetObject(hbitmap2, sizeof(BITMAP), &bm2);
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case tank:
		{
			if (GetAsyncKeyState(VK_UP) & 0x8000)
			{
				if (tank1.direction != UP)
					tank1.direction = UP;
				else if ((tank1.locationY - tank1.height / 2 - 10) < rect.top)
					tank1.locationY = rect.top + tank1.height / 2;
				else tank1.locationY -= 10;
			}
			else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				if (tank1.direction != DOWN)
					tank1.direction = DOWN;
				else if ((tank1.locationY + tank1.height / 2 + 10) > rect.bottom)
					tank1.locationY = rect.bottom - tank1.height / 2;
				else tank1.locationY += 10;
			}
			else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			{
				if (tank1.direction != LEFT)
					tank1.direction = LEFT;
				else if ((tank1.locationX - tank1.width / 2 - 10) < rect.left)
					tank1.locationX = rect.left + tank1.width / 2;
				else tank1.locationX -= 10;
			}
			else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			{
				if (tank1.direction != RIGHT)
					tank1.direction = RIGHT;
				else if ((tank1.locationX + tank1.width / 2 + 10) > rect.right)
					tank1.locationX = rect.right - tank1.width / 2;
				else tank1.locationX += 10;
			}
			if (GetAsyncKeyState('W') & 0x8000)
			{
				if (tank2.direction != UP)
					tank2.direction = UP;
				else if ((tank2.locationY - tank2.height / 2 - 10) < rect.top)
					tank2.locationY = rect.top + tank2.height / 2;
				else tank2.locationY -= 10;
			}
			else if (GetAsyncKeyState('S') & 0x8000)
			{
				if (tank2.direction != DOWN)
					tank2.direction = DOWN;
				else if ((tank2.locationY + tank2.height / 2 + 10) > rect.bottom)
					tank2.locationY = rect.bottom - tank2.height / 2;
				else tank2.locationY += 10;
			}
			else if (GetAsyncKeyState('A') & 0x8000)
			{
				if (tank2.direction != LEFT)
					tank2.direction = LEFT;
				else if ((tank2.locationX - tank2.width / 2 - 10) < rect.left)
					tank2.locationX = rect.left + tank2.width / 2;
				else tank2.locationX -= 10;
			}
			else if (GetAsyncKeyState('D') & 0x8000)
			{
				if (tank2.direction != RIGHT)
					tank2.direction = RIGHT;
				else if ((tank2.locationX + tank2.width / 2 + 10) > rect.right)
					tank2.locationX = rect.right - tank2.width / 2;
				else tank2.locationX += 10;
			}
			break;
		}
		case bullet:
		{
			if (tank1.bullet_head != NULL)
				(*(tank1.bullet_head)).Move(rect);
			if (tank2.bullet_head != NULL)
				(*(tank2.bullet_head)).Move(rect);
			break;
		}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_ERASEBKGND:
	{
		return true;
		break;
	}
	//case WM_ACTIVATE:
	//{
	//}
	case WM_KILLFOCUS:
	{
		KillTimer(hWnd, tank);
		KillTimer(hWnd, bullet);
		break;
	}
	case WM_SETFOCUS:
	{
		SetTimer(hWnd, tank, 100, NULL);
		SetTimer(hWnd, bullet, 100, NULL);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDB_ONE:
		{
			KillTimer(hWnd, tank);
			KillTimer(hWnd, bullet);
			int i = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PAUSE), hWnd, Pause);
			switch (i) {
			case 20:	//重新开始
				Init_all();
			case 10:	//回到游戏
				SetTimer(hWnd, tank, 100, NULL);
				SetTimer(hWnd, bullet, 100, NULL);
				break;
			case 30:	//回到主菜单
				KillTimer(hWnd, tank);
				KillTimer(hWnd, bullet);
				Return_To_Mune();
				break;
			}
			break;
		}

		case IDB_TWO: {
			Init_all();
			ShowWindow(hwndButton2, SW_HIDE);
			ShowWindow(hwndButton3, SW_HIDE);
			ShowWindow(hwndButton4, SW_HIDE);
			SetTimer(hWnd, tank, 100, NULL);
			SetTimer(hWnd, bullet, 100, NULL);
			ShowWindow(hwndButton1, SW_SHOW);
			isstart = 1;
			break;
		}
		case IDB_THREE: {break; }
		case IDB_FOUR://退出游戏
		{
			DestroyWindow(hWnd);
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_KEYDOWN:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		switch (wParam)
		{
			//	case VK_UP:
			//		if (tank1.direction != UP)
			//			tank1.direction = UP;
			//		else if ((tank1.locationY - tank1.height / 2 - 10) < rect.top)
			//			tank1.locationY = rect.top + tank1.height / 2;
			//		else tank1.locationY -= 10;
			//		break;
			//	case VK_DOWN:
			//		if (tank1.direction != DOWN)
			//			tank1.direction = DOWN;
			//		else if ((tank1.locationY + tank1.height / 2 + 10) > rect.bottom)
			//			tank1.locationY = rect.bottom - tank1.height / 2;
			//		else tank1.locationY += 10;
			//		break;
			//	case VK_LEFT:
			//		if (tank1.direction != LEFT)
			//			tank1.direction = LEFT;
			//		else if ((tank1.locationX - tank1.width / 2 - 10) < rect.left)
			//			tank1.locationX = rect.left + tank1.width / 2;
			//		else tank1.locationX -= 10;
			//		break;
			//	case VK_RIGHT:
			//		if (tank1.direction != RIGHT)
			//			tank1.direction = RIGHT;
			//		else if ((tank1.locationX + tank1.width / 2 + 10) > rect.right)
			//			tank1.locationX = rect.right - tank1.width / 2;
			//		else tank1.locationX += 10;
			//		break;
		case VK_OEM_2:
			if (tank1.isalive == true)
				tank1.Addbullet();
			break;
		case VK_SPACE:
			if (tank2.isalive == true)
				tank2.Addbullet();
			break;
		}
		//InvalidateRect(hWnd, NULL, TRUE);
	}


	case WM_PAINT:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		{
			//Rectangle(hdc, 20, 20, 40, 40);
			//Ellipse(hdc, 200, 200, 400, 400);
			//MoveToEx(hdc, 100, 100, NULL);
			//LineTo(hdc, 150, 150);
			//LineTo(hdc, 150, 200);
			//LineTo(hdc, 400, 400);
			//HPEN Pen1 = CreatePen(PS_SOLID, 10, RGB(255, 0, 0));
			//SelectObject(hdc, Pen1);
			//Rectangle(hdc, 400, 400, 500, 500);
			//LineTo(hdc, 500, 500);
			//HPEN Pen2 = CreatePen(PS_DASH, 1, RGB(0, 0, 255));
			//SelectObject(hdc, Pen2);
			//LineTo(hdc, 600, 500);

			if (!isstart) {
				SetBkColor(hdc, RGB(0, 0, 0));
			}

			else {
				hdcmem = CreateCompatibleDC(hdc);
				HBITMAP _hMemoryBMP = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
				SelectObject(hdcmem, _hMemoryBMP);
				SelectObject(hdcmem, WHITE_BRUSH);
				Rectangle(hdcmem, -10, -10, rect.right, rect.bottom);

				if (tank1.bullet_head)
					(*(tank1.bullet_head)).Drawbullet(bullet_Pen, bullet_Brush);
				if (tank2.bullet_head)
					(*(tank2.bullet_head)).Drawbullet(bullet_Pen, bullet_Brush);

				if (tank1.isalive)
					tank1.DrawTank(rect, hbitmap1, bm1);
				if (tank2.isalive)
					tank2.DrawTank(rect, hbitmap2, bm2);

				if (!BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hdcmem, 0, 0, SRCCOPY))
					MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);

				DeleteObject(_hMemoryBMP);
				DeleteDC(hdcmem);
				ReleaseDC(hWnd, hdc);
			}
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

INT_PTR CALLBACK Pause(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		RECT Father_rect;
		RECT My_rect;
		GetWindowRect(_hwnd, &Father_rect);
		GetClientRect(hDlg, &My_rect);
		SetWindowPos(
			hDlg,
			_hwnd,
			(Father_rect.right - My_rect.right) / 2, (Father_rect.bottom - My_rect.bottom) / 2,
			100, 200,
			SWP_NOSIZE
		);
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wParam) {
		case IDC_BUTTON1:
			EndDialog(hDlg, 10);
			break;
		case IDC_BUTTON2:
			EndDialog(hDlg, 20);
			break;
		case IDC_BUTTON3:
			EndDialog(hDlg, 30);
			break;
		default: return DefWindowProc(hDlg, message, wParam, lParam);
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(hDlg, 10);
		break;
	}
	}
	return (INT_PTR)FALSE;
}

