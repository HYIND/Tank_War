// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "Network.h"
#include "D2D_Scene.h"
#include "Game.h"
//#include "box2d/box2d.h"

static HBRUSH text_brush = CreateSolidBrush(RGB(230, 230, 230));

using namespace D2D1;

#define MAX_LOADSTRING 100

// 全局变量:
wstring my_userid = L"unname";

HRESULT hr = S_OK;

HDC hdc;
bool Hall_IOCP_flag = false;
bool Game_IOCP_flag = false;
bool reverse_in = false;

HANDLE Hall_hIOCP;
PPER_IO_DATA Hall_pPerIO;
HANDLE Game_hIOCP;
PPER_IO_DATA Game_pPerIO;

HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING] = L"Tank War";                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND _hwnd;
HWND Focus_hwnd;
RECT _rect;

WSADATA wsa;
sockaddr_in addr_info;
SOCKET mysocket = INVALID_SOCKET;
char buffer[1024];

void Init_all_Resource()
{
	Init_D2DResource();
	Init_Style();
	Init_Map();
}

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Pause(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    HALL(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ROOM(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GetID_Proc(HWND, UINT, WPARAM, LPARAM);

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_Tank_War));
	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(MYCURSOR));
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_Tank_War));

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
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_EX_COMPOSITED | WS_EX_LAYERED,
		CW_USEDEFAULT, 0, 1200, 675, nullptr, nullptr, hInstance, nullptr);

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

DWORD WINAPI Render_Thread()
{
	timeBeginPeriod(1);
	timeInOneFps = 1.0 / Fps; // 每秒60帧，则1帧就是约16毫秒
	PAINTSTRUCT ps;
	hdc = BeginPaint(_hwnd, &ps);
	RECT rect;
	GetClientRect(_hwnd, &rect);

	_LARGE_INTEGER time_now;
	_LARGE_INTEGER time_last;
	double dqFreq;		//计时器频率
	LARGE_INTEGER f;	//计时器频率
	QueryPerformanceFrequency(&f);
	dqFreq = (double)f.QuadPart;
	QueryPerformanceCounter(&time_last);//获取计时器跳数
	int i = 0;
	while (true)
	{
		QueryPerformanceCounter(&time_now);
		while ((time_now.QuadPart - time_last.QuadPart) / dqFreq < timeInOneFps)
		{
			QueryPerformanceCounter(&time_now);
			i = (timeInOneFps - (time_now.QuadPart - time_last.QuadPart) / dqFreq) * 1000;
			if (i > 0)
				Sleep(i);
		}
		time_last = time_now;
		{
			pRenderTarget->BeginDraw();
			pRenderTarget->Clear(ColorF(1, 1, 1, 1));
			if (status != STATUS::Game_Status) {
				pRenderTarget->DrawBitmap(OP_pBitmap, D2D1::RectF(0, 0, rect.right, rect.bottom));
				if (FAILED(hr))
				{
					MessageBox(NULL, L"Draw failed!", L"Error", 0);
				}
			}
			if (status == STATUS::Hall_Status || status == STATUS::Room_Status || (status == STATUS::Game_Status && isonline_game))
			{
				wstring ws = to_wstring(delay) + L"ms";
				const wchar_t* delay_ch = ws.c_str();
				pRenderTarget->DrawText(
					delay_ch,
					wcslen(delay_ch),
					pPing_Format,
					DelayRect,
					pMain_Brush
				);
			}
			if (status == STATUS::Game_Status)
			{
				Cur_Game->Draw();
			}
			if (CurScene)
				CurScene->DrawScene();
			hr = pRenderTarget->EndDraw();
		}
		//画点画线的各种处理
	}
	EndPaint(_hwnd, &ps);
	return 0;
}

enum { _game, _online, hall_refrash, reconnect, ping, room_refrash };	//定时器ID
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
		Init_all_Resource();

		if (FAILED(hr))
		{
			MessageBox(hWnd, _T("位图加载失败"), L"Error", MB_OK);
			DestroyWindow(hWnd);
		}
		CurScene = SMain;

		thread T(Render_Thread);
		T.detach();
		break;
	}

	case WM_TIMER:
	{
		switch (wParam)
		{
		case _online:
		{
			if (isonline_game)
			{
				Cur_Game->online();
			}
			break;
		}
		case _game:
		{
			if (GetFocus() == _hwnd || isonline_game)
			{
				Cur_Game->Move();
			}
			//if (GetFocus() == _hwnd)
			//{
			//	Cur_Game->Move();
			//}
			//else if (isonline_game)
			//{
			//	Cur_Game->Move();
			//}
			break;
		}
		case hall_refrash:
			Get_Hallinfo();
			break;
		case room_refrash:
			Get_Room_Info();
			break;
		case ping:
			send_pingmessage();
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_ERASEBKGND:
	{
		return true;
		break;
	}

	case WM_KILLFOCUS:
	{
		if (status == STATUS::Game_Status && !isonline_game)
		{
			KillTimer(hWnd, _game);
		}
		break;
	}

	case WM_SETFOCUS:
	{
		if (status == STATUS::Game_Status && !isonline_game)
		{
			SetTimer(hWnd, _game, 20, NULL);
		}
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (status)
		{
		case STATUS::Main:
		{
			switch (wmId)
			{
				// 开始游戏
			case IDB_LOCALGAME: {
				Cur_Game->Init_Game();
				SetTimer(hWnd, _game, 20, NULL);
				isonline_game = false;
				Set_CurScene(STATUS::Game_Status);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
							  // 联机大厅
			case IDB_ENTERHALL: {
				if (!DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_Userid), hWnd, (DLGPROC)GetID_Proc))
					break;
				if (!Init_Hall())
					break;
				setmyuserid();
				Show_Hall(true);
				CurScene = SHall;
				status = STATUS::Hall_Status;
				SetTimer(hWnd, reconnect, 5000, NULL);
				SetTimer(hWnd, hall_refrash, 4000, NULL);
				SetTimer(hWnd, ping, 2000, NULL);
				UpdateWindow(hWnd);
				break;
			}
							  // 设置
			case IDB_OPTION: {
				CurScene = SOption;
				status = STATUS::Option;
				break;
			}
						   // 退出游戏
			case IDB_QUITGAME:
			{
				DestroyWindow(hWnd);
				break;
			}
			}
			break;
		}
		case STATUS::Option:
		{
			switch (wmId)
			{
			case IDB_EXITOPTION:
			{
				CurScene = SMain;
				status = STATUS::Main;
				break;
			}
			case IDB_SETFPS_30:
			{
				Set_Fps(30.0);
				break;
			}
			case IDB_SETFPS_60:
			{
				Set_Fps(60.0);
				break;
			}
			case IDB_SETFPS_144:
			{
				Set_Fps(144.0);
				break;
			}
			default:
				break;
			}

		}
		case STATUS::Hall_Status:
		{
			switch (wmId)
			{
			case Enterroom:
			{
				KillTimer(hWnd, hall_refrash);
				SetTimer(hWnd, room_refrash, 1000, NULL);
				Set_CurScene(STATUS::Room_Status);
				UpdateWindow(_hwnd);
				break;
			}
			// 刷新
			case IDB_REFRESH:
			{
				Get_Hallinfo();
				break;
			}
			// 加入房间
			case IDB_ENTERROOM:
			{
				int index = SendMessage(Hall_room_list, LB_GETCURSEL, 0, 0);
				TCHAR buff[255];
				SendMessage(Hall_room_list, LB_GETTEXT, index, (LPARAM)buff);
				Enter_Room(index);
				break;
			}
			// 离开大厅
			case IDB_EXITHALL:
				closesocket(mysocket);
				KillTimer(hWnd, reconnect);
				KillTimer(hWnd, hall_refrash);
				KillTimer(hWnd, ping);
				Set_CurScene(STATUS::Main);
				UpdateWindow(_hwnd);
				break;
			case IDB_HALL_SEND:
			{
				wchar_t temp[1024] = { '\0' };
				GetWindowTextW(Hall_edit_in, temp, 1024);
				wstring str = temp;
				Send_Hall_Message(str);
				break;
			}
			// 创建房间
			case IDB_CREATEROOM:
			{
				KillTimer(hWnd, hall_refrash);
				SetTimer(hWnd, room_refrash, 1000, NULL);
				Create_Room();
				UpdateWindow(_hwnd);
				break;
			}
			}
			break;
		}
		case STATUS::Room_Status:
		{
			switch (wmId)
			{
			case IDB_STARTGAME:
			{
				if (host == false)
				{
					MessageBox(hWnd, L"您不是房主，请等待房主开始游戏", NULL, MB_OK);
					break;
				}
				send(mysocket, "StartGame", 1023, 0);
				break;
			}
			case IDB_READY:
			{
				Room_Ready();
				break;
			}
			case IDB_CANCELREADY:
			{
				Room_CancelReady();
				break;
			}
			case DISBANDINROOM:
			{
				KillTimer(hWnd, room_refrash);
				MessageBox(hWnd, L"房主已将房间解散！", NULL, MB_OK);
				Set_CurScene(STATUS::Hall_Status);
				SetTimer(hWnd, hall_refrash, 4000, NULL);
				UpdateWindow(hWnd);
				break;
			}
			case IDB_EXITROOM:
			{
				KillTimer(hWnd, room_refrash);
				send(mysocket, "QuitRoom", 1023, 0);
				Set_CurScene(STATUS::Hall_Status);
				SetTimer(hWnd, hall_refrash, 4000, NULL);
				UpdateWindow(hWnd);
				break;
			}
			case START:
			{
				KillTimer(hWnd, room_refrash);
				SetTimer(hWnd, _game, 20, NULL);
				SetTimer(hWnd, _online, 25, NULL);

				isonline_game = true;
				isstart = true;
				Cur_Game->Init_Game();
				Set_CurScene(STATUS::Game_Status);
				if (host)
				{
					my_tank_location = 1;
				}
				else my_tank_location = 2;
				break;
			}
			case IDB_ROOM_SEND:
			{
				wchar_t temp[1024] = { '\0' };
				GetWindowTextW(Room_edit_in, temp, 1024);
				wstring str = temp;
				if (!str.empty())
					Send_Room_Message(str);
				break;
			}
			}
			break;
		}
		case STATUS::Game_Status:
		{
			switch (wmId)
			{
				// 暂停
			case IDB_PAUSE:
			{
				KillTimer(hWnd, _game);
				int i = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PAUSE), hWnd, Pause);
				switch (i) {
				case 10:	//回到游戏
					SetTimer(hWnd, _game, 20, NULL);
					break;
				case 20:	//重新开始
					Cur_Game->Init_Game();
					SetTimer(hWnd, _game, 20, NULL);
					break;
				case 30:	//回到主菜单
					Set_CurScene(STATUS::Main);
					break;
				}
				break;
			}
			case WIN:
			{
				CurScene = SWinGame;
				break;
			}
			case FAIL:
			{
				CurScene = SFailGame;
				break;
			}
			case ReturnInEndGame:
			{
				KillTimer(_hwnd, _game);
				KillTimer(_hwnd, _online);
				SetTimer(hWnd, reconnect, 5000, NULL);
				//SetTimer(hWnd, hall_refrash, 4000, NULL);
				SetTimer(hWnd, room_refrash, 1000, NULL);
				Set_CurScene(STATUS::Room_Status);
				Show_Room(TRUE);
				break;
			}
			case DISBANDINEND:
			{
				MessageBox(hWnd, L"房主已将房间解散！", NULL, MB_OK);
				KillTimer(_hwnd, _game);
				KillTimer(_hwnd, _online);
				SetTimer(hWnd, reconnect, 5000, NULL);
				SetTimer(hWnd, hall_refrash, 4000, NULL);
				Set_CurScene(STATUS::Hall_Status);
			}
			}
			break;
		}
		default:
		{
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				SafeRelease(pRenderTarget);
				SafeRelease(pD2DFactory);
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}
		}
	}
	case WM_MOUSEMOVE:
	{
		MoveX = LOWORD(lParam);
		MoveY = HIWORD(lParam);
		if (CurScene)
			CurScene->Move();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		ClickX = LOWORD(lParam);
		ClickY = HIWORD(lParam);
		if (CurScene)
			CurScene->Click();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	break;

	//case WM_KEYDOWN:
	//{
	//	RECT rect;
	//	GetClientRect(hWnd, &rect);
	//	switch (wParam)
	//	{
	//	}
	// break;
	//}
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLOREDIT:
	//{
	//	HWND t_hwnd = (HWND)lParam;
	//	if (t_hwnd == Hall_edit_in || t_hwnd == edit_hall || t_hwnd == Hall_room_list || t_hwnd == Hall_user_list)
	//	{
	//		//RECT text_rect;
	//		//GetClientRect(t_hwnd, &text_rect);
	//		//pRenderTarget_t->BeginDraw();
	//		//pRenderTarget_t->Clear(ColorF(1, 1, 1, 1));
	//		//pRenderTarget_t->DrawBitmap(TEXT_pBitmap, D2D1::RectF(text_rect.left, text_rect.top, text_rect.right, text_rect.bottom),0.4f);
	//		//pRenderTarget_t->EndDraw();
	//		HDC t_hdc = (HDC)wParam;
	//		SetTextColor(t_hdc, RGB(0, 0, 0));   //设置文字颜色
	//		SetBkColor(t_hdc, RGB(230, 230, 230));//设置文字背景颜色
	//		SetBkMode(t_hdc, TRANSPARENT);;
	//		////RECT rect;
	//		////GetClientRect(hWnd, &rect);
	//		////HDC m_hdc = GetDC(hWnd);
	//		//////通过SetStretchBltMode的设置能使StretchBlt在缩放图像更加清晰
	//		////SetStretchBltMode((HDC)wParam, COLORONCOLOR);
	//		////StretchBlt((HDC)wParam, 0, 0, rect.right, rect.bottom, hdcmem, 0, 0, bm1.bmWidth, bm1.bmHeight, SRCCOPY);
	//		return (INT_PTR)text_brush;
	//	}
	//	break;
	//}
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORSTATIC:
	//{
	//	HDC hdcStatic = (HDC)wParam;
	//	SetTextColor(hdcStatic, RGB(255, 255, 255));   //设置文字颜色
	//	SetBkColor(hdcStatic, RGB(0, 0, 0));//设置文字背景颜色
	//	SetBkMode(hdcStatic, TRANSPARENT);//设置控件背景为透明
	//	return (INT_PTR)(HBRUSH(GetStockObject(NULL_BRUSH)));//设置控件背景颜色为灰色
	//	break;
	//}

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

// 暂停窗口
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

// 登陆前设置ID窗口
BOOL CALLBACK GetID_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		userid_in = GetDlgItem(hDlg, Userid_in);
		SetWindowTextW(userid_in, my_userid.c_str());
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			wchar_t temp[17] = { '\0' };
			GetWindowTextW(userid_in, temp, 17);
			my_userid = temp;
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		case IDCANCEL:
		{
			EndDialog(hDlg, FALSE);
			return FALSE;
		}
		}
	}
	}
	return FALSE;
}