// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "WindowsProject1.h"
#include "Method.h"
#include <comdef.h>

//using namespace std;

//#include "GdiPlus.h"

#define MAX_LOADSTRING 100
#define DEFAULT_PORT 2336
#define SERVER_IP "175.178.90.119"

//extern vector <tank_info*> tank_list;

// 全局变量:
Tank tank1(80, 80);
Tank tank2(80, 80);
Tank* mytank = &tank1;
Tank* optank = &tank2;

Tank* my_in = &tank1;
Tank* op_in = &tank2;


HDC hdcmem, hdc;
bool isstart = false;
bool host = false;
bool isonline_game = false;
bool Hall_IOCP_flag = false;
bool Game_IOCP_flag = false;
bool reverse_in = true;

HANDLE Hall_hIOCP;
PPER_IO_DATA Hall_pPerIO;
HANDLE Game_hIOCP;
PPER_IO_DATA Game_pPerIO;

//加载位图
HBITMAP hbitmap1 = (HBITMAP)LoadImage(NULL, L"jiaran.bmp", IMAGE_BITMAP, mytank->width, mytank->height, LR_DEFAULTSIZE | LR_LOADFROMFILE);
HBITMAP hbitmap2 = (HBITMAP)LoadImage(NULL, L"beila2.bmp", IMAGE_BITMAP, optank->width, optank->height, LR_DEFAULTSIZE | LR_LOADFROMFILE);
HBITMAP op_bitmap = (HBITMAP)LoadImage(NULL, L"OP_BR.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
BITMAP bm1;
BITMAP bm2;
BITMAP op_bm;
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING] = L"Tank War";                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND _hwnd;
RECT _rect;

HWND hwndButton1;
HWND hwndButton2;
HWND hwndButton3;
HWND hwndButton4;
HWND hwndButton5;
HWND hwndButton6;

HWND Hall;
HWND Room;

HPEN tank_Pen = CreatePen(PS_SOLID, 0, RGB(100, 100, 200));
HBRUSH tank_Brush = CreateSolidBrush(RGB(255, 238, 114));
HPEN bullet_Pen = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
HBRUSH bullet_Brush = CreateSolidBrush(RGB(255, 0, 0));


WSADATA wsa;
sockaddr_in addr_info;
SOCKET mysocket = INVALID_SOCKET;
char buffer[1024];

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Pause(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    HALL(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ROOM(HWND, UINT, WPARAM, LPARAM);


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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	//op_bitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(BITMAP_BK));

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
enum { _tank, _bullet, online };
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
		if (!op_bitmap)
		{
			MessageBox(hWnd, _T("位图加载失败"), NULL, MB_OK);
		}
		mytank->InitTank(rect.left + mytank->width / 2 + 20, (rect.bottom - rect.top) / 2, RIGHT);
		optank->InitTank(rect.right - optank->width / 2 - 20, (rect.bottom - rect.top) / 2, LEFT);
		Get_Init_UI(_hwnd);
		Get_Initinfo();
		GetObject(hbitmap1, sizeof(BITMAP), &bm1);
		GetObject(hbitmap2, sizeof(BITMAP), &bm2);
		GetObject(op_bitmap, sizeof(BITMAP), &op_bm);
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case online:
		{
			if (isonline_game)
			{
				send_location(mytank);
				send_bullet(mytank->bullet_head);
			}
		}
		case _tank:
		{
			if (GetFocus() != _hwnd)
				break;
			if (reverse_in)
			{
				mytank->Tank1_Move(rect);
				if (!isonline_game)
					optank->Tank2_Move(rect);
				break;
			}
			else {
				mytank->Tank2_Move(rect);
				if (!isonline_game)
					optank->Tank1_Move(rect);
				break;
			}
		}
		case _bullet:
		{
			if (mytank->bullet_head)
				(*(mytank->bullet_head)).Move(rect);
			if (!isonline_game)
			{
				if (optank->bullet_head)
					(*(optank->bullet_head)).Move(rect);
			}
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
		//KillTimer(hWnd, tank);
		//KillTimer(hWnd, bullet);
		break;
	}
	case WM_SETFOCUS:
	{
		//SetTimer(hWnd, tank, 100, NULL);
		//SetTimer(hWnd, bullet, 100, NULL);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:

		switch (wmId)
		{
			// 暂停
		case IDB_ONE:
		{
			KillTimer(hWnd, _tank);
			KillTimer(hWnd, _bullet);
			int i = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PAUSE), hWnd, Pause);
			switch (i) {
			case 20:	//重新开始
				Init_all();
			case 10:	//回到游戏
				SetTimer(hWnd, _tank, 100, NULL);
				SetTimer(hWnd, _bullet, 100, NULL);
				break;
			case 30:	//回到主菜单
				KillTimer(hWnd, _tank);
				KillTimer(hWnd, _bullet);
				Return_To_Mune();
				break;
			}
			break;
		}

		// 开始游戏
		case IDB_TWO: {
			Init_all();
			Hide_Main_UI();
			SetTimer(hWnd, _tank, 100, NULL);
			SetTimer(hWnd, _bullet, 100, NULL);
			ShowWindow(hwndButton1, SW_SHOW);
			isstart = true;
			isonline_game = false;
			break;
		}
					// 联机大厅
		case IDB_THREE: {
			Hide_Main_UI();
			Hall = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_HALL), hWnd, HALL);
			ShowWindow(Hall, SW_SHOW);
			break;
		}
					  // 设置
		case IDB_FOUR: {break; }
		case IDB_FIVE:// 退出游戏
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
		case VK_SPACE:
		{
			if (reverse_in)
			{
				if (mytank->isalive)
					mytank->Addbullet();
				break;
			}
			else if (!isonline_game)
			{
				if (optank->isalive)
					optank->Addbullet();
				break;
			}
		}
		case VK_OEM_2:
		{
			if (reverse_in)
			{
				if (!isonline_game)
				{
					if (optank->isalive)
						optank->Addbullet();
					break;
				}
			}
			else
			{
				if (mytank->isalive)
					mytank->Addbullet();
				break;
			}
		}
		//InvalidateRect(hWnd, NULL, TRUE);
		}
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

			hdcmem = CreateCompatibleDC(hdc);
			HBITMAP _hMemoryBMP = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.right - rect.left);
			SelectObject(hdcmem, _hMemoryBMP);
			if (!isstart) {
				//HBRUSH hBlackBrush = (HBRUSH)::GetStockObject(BLACK_BRUSH);
				SelectObject(hdcmem, GetStockObject(GRAY_BRUSH));
				Rectangle(hdcmem, -10, -10, rect.right, rect.bottom);
				HDC op_hdc = CreateCompatibleDC(hdcmem);
				SelectObject(op_hdc, op_bitmap);
				StretchBlt(hdcmem, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, op_hdc, 0, 0, op_bm.bmWidth, op_bm.bmHeight, SRCCOPY);
				DeleteDC(op_hdc);
			}

			else {
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
			}
			if (!BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hdcmem, 0, 0, SRCCOPY))
				MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
			ReleaseDC(hWnd, hdc);
			DeleteObject(_hMemoryBMP);
			DeleteDC(hdcmem);
		}
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		closesocket(mysocket);
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


enum { refrash, reconnect };
HWND room_list;
HWND user_list;
HWND edit_in;
HWND edit_hall;
bool edit_in_focus = false;
INT_PTR CALLBACK HALL(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		SetTimer(hDlg, reconnect, 5000, NULL);
		SetTimer(hDlg, refrash, 3000, NULL);
		{
			RECT Father_rect;
			RECT My_rect;
			GetClientRect(_hwnd, &Father_rect);
			GetClientRect(hDlg, &My_rect);
			SetWindowPos(
				hDlg,
				NULL,
				Father_rect.left + (Father_rect.right - Father_rect.left) / 2 - My_rect.right / 2,
				Father_rect.top + (Father_rect.bottom - Father_rect.top) / 2 - My_rect.bottom / 2,
				100, 200,
				SWP_NOSIZE
			);
		}
		user_list = GetDlgItem(hDlg, IDC_LIST1);
		room_list = GetDlgItem(hDlg, IDC_LIST2);
		edit_hall = GetDlgItem(hDlg, IDC_EDIT1);
		edit_in = GetDlgItem(hDlg, IDC_EDIT2);
		{
			WSAStartup(MAKEWORD(2, 2), &wsa);
			ZeroMemory(&addr_info, sizeof(addr_info));
			addr_info.sin_family = AF_INET;
			addr_info.sin_port = htons(DEFAULT_PORT);
			inet_pton(AF_INET, SERVER_IP, &(addr_info.sin_addr.S_un.S_addr));

			memset(buffer, '\0', 1024);
			mysocket = WSASocket(addr_info.sin_family, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			connect(mysocket, (struct sockaddr*)&addr_info, sizeof(struct sockaddr));

			Hall_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
			CreateIoCompletionPort((HANDLE)mysocket, Hall_hIOCP, NULL, 0);
			Hall_pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
			Hall_pPerIO->nOperationType = OP_READ;
			WSABUF buf;
			buf.buf = Hall_pPerIO->buf;
			buf.len = 1024;
			DWORD dwRecv;
			DWORD dwFlags = 0;
			WSARecv(mysocket, &buf, 1, &dwRecv, &dwFlags, &Hall_pPerIO->ol, NULL);
			thread T(Recv_Thread, Hall_pPerIO, LPVOID(Hall_hIOCP));
			T.detach();
		}
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wParam)
		{
		case Enterroom:
		{
			ShowWindow(Hall, SW_HIDE);
			Room = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_ROOM), _hwnd, ROOM);
			ShowWindow(Room, SW_SHOW);
			break;
		}
		// 刷新
		case IDC_BUTTON1:
			Get_Hallinfo();
			break;
			// 加入房间
		case IDC_BUTTON2:
		{
			int index = SendMessage(room_list, LB_GETCURSEL, 0, 0);
			//TCHAR buff[255];
			//SendMessage(room_list,LB_GETTEXT, index, (LPARAM)buff);
			//wstring wtemp = string2wstring(to_string(index));
			//MessageBox(hDlg, buff, &wtemp[0], MB_OK);
			Enter_Room(index);
			break;
		}
		// 离开大厅
		case IDC_BUTTON3:
			closesocket(mysocket);
			DestroyWindow(hDlg);
			Show_Main_UI();
			break;
		case IDC_BUTTON4:
		{
			wstring temp;
			GetWindowTextW(edit_in, &(temp[0]), 1024);
			Send_Message(temp);
			break;
		}
		// 创建房间
		case IDC_BUTTON5:
		{
			ShowWindow(Hall, SW_HIDE);
			Create_Room();
			Room = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_ROOM), _hwnd, ROOM);
			host = true;
			ShowWindow(Room, SW_SHOW);
			break;
		}
		if (HIWORD(wParam) == EN_SETFOCUS)
		{
			if ((HWND)lParam == edit_in) {
				edit_in_focus = true;
			}
		}
		if (HIWORD(wParam) == EN_KILLFOCUS)
		{
			if ((HWND)lParam == edit_in) {
				edit_in_focus = true;
			}
		}
		default: return DefWindowProc(hDlg, message, wParam, lParam);
		}
		break;
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case refrash:
			Get_Hallinfo();
			break;
			//case reconnect:
			//if (!isconnecting())			
			//	if (connect(mysocket, (struct sockaddr*)&addr_info, sizeof(struct sockaddr)) == -1)
			//	{
			//		(int)SendMessage(user_list, LB_RESETCONTENT, 0, 0);
			//		(int)SendMessage(user_list, LB_ADDSTRING, 0, (LPARAM) L"已断开连接，检查网络！");
			//	}
			break;
		}
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_RETURN:
			if (edit_in_focus) {
				wstring temp;
				GetWindowTextW(edit_in, &(temp[0]), 1024);
				Send_Message(temp);
				break;
			}
		}
	}
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ROOM(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		{
			RECT Father_rect;
			RECT My_rect;
			GetClientRect(_hwnd, &Father_rect);
			GetClientRect(hDlg, &My_rect);
			SetWindowPos(
				hDlg,
				NULL,
				Father_rect.left + (Father_rect.right - Father_rect.left) / 2 - My_rect.right / 2,
				Father_rect.top + (Father_rect.bottom - Father_rect.top) / 2 - My_rect.bottom / 2,
				100, 200,
				SWP_NOSIZE
			);
			{
				CreateWindow(L"STATIC", L"#101", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_BITMAP,
					My_rect.left + (My_rect.right - My_rect.left) / 8, My_rect.top + (My_rect.bottom - My_rect.top) / 6,
					(My_rect.right - My_rect.left) / 4, (My_rect.right - My_rect.left) / 4,
					hDlg,       // parent window
					(HMENU)IDB_Bitmap1,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);
				CreateWindow(L"STATIC", L"#102", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_BITMAP,
					My_rect.left + ((My_rect.right - My_rect.left) / 8) * 5, My_rect.top + (My_rect.bottom - My_rect.top) / 6,
					(My_rect.right - My_rect.left) / 4, (My_rect.right - My_rect.left) / 4,
					hDlg,       // parent window
					(HMENU)IDB_Bitmap2,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);
			}

			{
				CreateWindow(
					L"BUTTON",	// predefined class    不区分大小写
					L"<",       // button text
					WS_CHILD | WS_VISIBLE,  // styles     注意 如果样式写错了 Button 将不会正常显示

				   // Size and position values are given explicitly, because
				   // the CW_USEDEFAULT constant gives zero values for buttons.
					My_rect.left + (My_rect.right - My_rect.left) / 8,         // starting x position
					My_rect.top + (My_rect.bottom - My_rect.top) / 6 + (My_rect.right - My_rect.left) / 4 + 20,         // starting y position
					40,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_ONE,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);      // pointer not needed
				CreateWindow(
					L"BUTTON",	// predefined class    不区分大小写
					L">",       // button text
					WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

				   // Size and position values are given explicitly, because
				   // the CW_USEDEFAULT constant gives zero values for buttons.
					My_rect.left + (My_rect.right - My_rect.left) / 8 + (My_rect.right - My_rect.left) / 4 - 40,         // starting x position
					My_rect.top + (My_rect.bottom - My_rect.top) / 6 + (My_rect.right - My_rect.left) / 4 + 20,         // starting y position
					40,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_TWO,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);      // pointer not needed
				CreateWindow(
					L"BUTTON",	// predefined class    不区分大小写
					L"<",       // button text
					WS_CHILD | WS_VISIBLE,  // styles     注意 如果样式写错了 Button 将不会正常显示

				   // Size and position values are given explicitly, because
				   // the CW_USEDEFAULT constant gives zero values for buttons.
					My_rect.left + (My_rect.right - My_rect.left) / 8 * 5,         // starting x position
					My_rect.top + (My_rect.bottom - My_rect.top) / 6 + (My_rect.right - My_rect.left) / 4 + 20,         // starting y position
					40,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_THREE,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);      // pointer not needed
				CreateWindow(
					L"BUTTON",	// predefined class    不区分大小写
					L">",       // button text
					WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

				   // Size and position values are given explicitly, because
				   // the CW_USEDEFAULT constant gives zero values for buttons.
					My_rect.left + (My_rect.right - My_rect.left) / 8 * 5 + (My_rect.right - My_rect.left) / 4 - 40,         // starting x position
					My_rect.top + (My_rect.bottom - My_rect.top) / 6 + (My_rect.right - My_rect.left) / 4 + 20,         // starting y position
					40,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_FOUR,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);      // pointer not needed
			}

			{
				CreateWindow(
					L"BUTTON",	// predefined class    不区分大小写
					L"开始游戏",       // button text
					WS_CHILD | WS_VISIBLE,  // styles     注意 如果样式写错了 Button 将不会正常显示

				   // Size and position values are given explicitly, because
				   // the CW_USEDEFAULT constant gives zero values for buttons.
					My_rect.right - 160,         // starting x position
					My_rect.bottom - 70,         // starting y position
					80,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_FIVE,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);      // pointer not needed
			}
		}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDB_FIVE:
		{
			if (host == false)
			{
				MessageBox(hDlg, L"您不是房主，请等待房主开始游戏", NULL, MB_OK);
				break;
			}
			send(mysocket, "startgame", 1023, 0);
			break;
		}
		case START:
		{
			KillTimer(Hall, refrash);
			Init_all();
			Hide_Main_UI();
			SetTimer(_hwnd, _tank, 100, NULL);
			SetTimer(_hwnd, _bullet, 100, NULL);
			SetTimer(_hwnd, online, 40, NULL);
			ShowWindow(hwndButton1, SW_SHOW);

			//Game_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
			//CreateIoCompletionPort((HANDLE)mysocket, Game_hIOCP, NULL, 0);
			//Game_pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
			//Game_pPerIO->nOperationType = OP_READ;
			//WSABUF buf;
			//buf.buf = Game_pPerIO->buf;
			//buf.len = 1024;
			//DWORD dwRecv;
			//DWORD dwFlags = 0;
			//WSARecv(mysocket, &buf, 1, &dwRecv, &dwFlags, &Game_pPerIO->ol, NULL);
			//thread T(Recv_Thread, Game_pPerIO, LPVOID(Game_hIOCP),Game_IOCP_flag);
			//T.detach();
			isonline_game = true;
			isstart = true;
			if (!host)
			{
				reverse_in = false;
				swap(mytank, optank);
			}
			break;
		}
		}
	}
	}
	}
	return (INT_PTR)FALSE;
}