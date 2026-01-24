// WindowsProject1.cpp : 定义应用程序的入口点。
//
#include "Render.h"
#include "Tank_AI.h"

#define MAX_LOADSTRING 100

//#define AITEST
// 全局变量:

bool reverse_in = false;

HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING] = L"Tank War";                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND _hwnd;
RECT _rect;
HDC hdc;

bool Init_all_Resource()
{
	LOGINFO("Init", "Init_All");
	ResFactory->InitResource();
	Init_Scene();
	Init_Style();
	Init_Map();
	Init_ComponentMap();
	return true;
}

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Pause(HWND, UINT, WPARAM, LPARAM);
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

enum { _game, _online, hall_refreash, reconnect, ping, room_refreash, sceneTick };	//定时器ID
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	GetClientRect(hWnd, &_rect);
	_hwnd = hWnd;
	switch (message)
	{
	case WM_CREATE:
	{
		bool result = Init_all_Resource();
		if (!result)
		{
			MessageBox(hWnd, _T("位图加载失败"), L"Error", MB_OK);
			DestroyWindow(hWnd);
		}
		_Scene::CurScene = _Scene::SMain;
		thread T(Render_Thread);
		T.detach();
		SetTimer(hWnd, sceneTick, 20, NULL);
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
				Game::Instance()->online();
			}
			break;
		}
		case _game:
		{
			Game::Instance()->Tick();

#ifdef AITEST
			static int count = 0;
			count++;
			if (count > 5)count -= 5;
			if (count % 5 == 0)
				AI_calculate();
			AI_Track();
#endif // AITEST

			break;
		}
		case hall_refreash:
			NetManager::Instance()->NetManager::Instance()->Get_Hallinfo();
			break;
		case room_refreash:
			NetManager::Instance()->Get_Room_Info();
			break;
		case ping:
			NetManager::Instance()->send_pingmessage();
			break;
		case sceneTick:
			_Scene::CurScene->Tick();
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
		if (Get_CurScene() == STATUS::Game_Status && !isonline_game)
		{
			KillTimer(hWnd, _game);
		}
		break;
	}

	case WM_SETFOCUS:
	{
		if (Get_CurScene() == STATUS::Game_Status && !isonline_game)
		{
			SetTimer(hWnd, _game, 20, NULL);
		}
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (Get_CurScene())
		{
		case STATUS::Main:
		{
			switch (wmId)
			{
				// 开始游戏
			case IDB_LOCALGAME: {
				Game::Instance()->Init_Game();
				SetTimer(hWnd, _game, 20, NULL);
				isonline_game = false;
				Set_CurScene(STATUS::Game_Status);
				InvalidateRect(hWnd, NULL, TRUE);
#ifdef AITEST
				AI_Init();
#endif // AITEST
				break;
			}
							  // 联机大厅
			case IDB_ENTERHALL: {
				if (!DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_Userid), hWnd, (DLGPROC)GetID_Proc))
					break;
				if (!NetManager::Instance()->Init_Hall())
					break;
				NetManager::Instance()->Send_my_userid();
				Set_CurScene(STATUS::Hall_Status);
				SetTimer(hWnd, reconnect, 5000, NULL);
				SetTimer(hWnd, hall_refreash, 4000, NULL);
				SetTimer(hWnd, ping, 2000, NULL);
				UpdateWindow(hWnd);
				break;
			}
							  // 设置
			case IDB_OPTION: {
				Set_CurScene(STATUS::Option);
				break;
			}
						   //地图编辑
			case IDB_MAPEDIT: {
				Set_CurScene(STATUS::MapEdit);
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
				Set_CurScene(STATUS::Main);
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
				KillTimer(hWnd, hall_refreash);
				SetTimer(hWnd, room_refreash, 1000, NULL);
				Set_CurScene(STATUS::Room_Status);
				UpdateWindow(_hwnd);
				break;
			}
			// 刷新
			case IDB_REFRESH:
			{
				NetManager::Instance()->Get_Hallinfo();
				break;
			}
			// 加入房间
			case IDB_ENTERROOM:
			{
				int index = SendMessage(_Scene::SHall->Hall_room_list, LB_GETCURSEL, 0, 0);
				TCHAR buff[255];
				SendMessage(_Scene::SHall->Hall_room_list, LB_GETTEXT, index, (LPARAM)buff);
				NetManager::Instance()->Enter_Room(index);
				break;
			}
			// 离开大厅
			case IDB_EXITHALL:
				NetManager::Instance()->close_connect();
				KillTimer(hWnd, reconnect);
				KillTimer(hWnd, hall_refreash);
				KillTimer(hWnd, ping);
				Set_CurScene(STATUS::Main);
				UpdateWindow(_hwnd);
				break;
			case IDB_HALL_SEND:
			{
				wchar_t temp[1024] = { '\0' };
				GetWindowTextW(_Scene::SHall->Hall_edit_in, temp, 1024);
				wstring str = temp;
				if (!str.empty())
					NetManager::Instance()->Send_Hall_Message(str);
				break;
			}
			// 创建房间
			case IDB_CREATEROOM:
			{
				KillTimer(hWnd, hall_refreash);
				SetTimer(hWnd, room_refreash, 1000, NULL);
				NetManager::Instance()->Create_Room();
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
				NetManager::Instance()->send_string("StartGame");
				break;
			}
			case IDB_READY:
			{
				NetManager::Instance()->Room_Ready();
				break;
			}
			case IDB_CANCELREADY:
			{
				NetManager::Instance()->Room_CancelReady();
				break;
			}
			case DISBANDINROOM:
			{
				KillTimer(hWnd, room_refreash);
				MessageBox(hWnd, L"房主已将房间解散！", NULL, MB_OK);
				Set_CurScene(STATUS::Hall_Status);
				SetTimer(hWnd, hall_refreash, 4000, NULL);
				UpdateWindow(hWnd);
				break;
			}
			case IDB_EXITROOM:
			{
				KillTimer(hWnd, room_refreash);
				NetManager::Instance()->send_string("QuitRoom");
				Set_CurScene(STATUS::Hall_Status);
				SetTimer(hWnd, hall_refreash, 4000, NULL);
				UpdateWindow(hWnd);
				break;
			}
			case START:
			{
				KillTimer(hWnd, room_refreash);
				SetTimer(hWnd, _game, 20, NULL);
				SetTimer(hWnd, _online, 25, NULL);

				isonline_game = true;
				isstart = true;
				Game::Instance()->Init_Game();
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
				GetWindowTextW(Scene_Room::Room_edit_in, temp, 1024);
				wstring str = temp;
				if (!str.empty())
					NetManager::Instance()->Send_Room_Message(str);
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
					Game::Instance()->Init_Game();
#ifdef AITEST
					AI_Init();
#endif // AITEST
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
				_Scene::CurScene = _Scene::SWinGame;
				break;
			}
			case FAIL:
			{
				_Scene::CurScene = _Scene::SFailGame;
				break;
			}
			case ReturnInEndGame:
			{
				KillTimer(_hwnd, _game);
				KillTimer(_hwnd, _online);
				SetTimer(hWnd, reconnect, 5000, NULL);
				//SetTimer(hWnd, hall_refreash, 4000, NULL);
				SetTimer(hWnd, room_refreash, 1000, NULL);
				Set_CurScene(STATUS::Room_Status);
				Scene_Room::Show_Room(TRUE);
				break;
			}
			case DISBANDINEND:
			{
				MessageBox(hWnd, L"房主已将房间解散！", NULL, MB_OK);
				KillTimer(_hwnd, _game);
				KillTimer(_hwnd, _online);
				SetTimer(hWnd, reconnect, 5000, NULL);
				SetTimer(hWnd, hall_refreash, 4000, NULL);
				Set_CurScene(STATUS::Hall_Status);
				break;
			}
			case ReLoadMap:
			{
				Game::Instance()->LoadLocalMap();
				break;
			}
			}
			break;
		}
		case STATUS::MapEdit:
		{
			switch (wmId)
			{
			case MAPEDIT_EXIT:
			{
				Set_CurScene(STATUS::Main);
				break;
			}

			case MAPEDIT_LOAD:
			{
				if (_Scene::CurScene != _Scene::SMapEdit)
					break;
				_Scene::SMapEdit->ReadMapFile();
				break;
			}

			case MAPEDIT_SAVE:
			{
				if (_Scene::CurScene != _Scene::SMapEdit)
					break;
				_Scene::SMapEdit->SaveMapFile();
				break;
			}
			}
			break;
		}
		default:
		{
			switch (wmId)
			{
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
		MousePos::MoveX = LOWORD(lParam);
		MousePos::MoveY = HIWORD(lParam);
		if (_Scene::CurScene)
			_Scene::CurScene->Move();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		MousePos::ClickX = LOWORD(lParam);
		MousePos::ClickY = HIWORD(lParam);
		if (_Scene::CurScene)
			_Scene::CurScene->Click(true, GetAsyncKeyState(VK_SHIFT) & 0x8000);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		MousePos::ClickX = LOWORD(lParam);
		MousePos::ClickY = HIWORD(lParam);
		if (_Scene::CurScene)
			_Scene::CurScene->Click(false, GetAsyncKeyState(VK_SHIFT) & 0x8000);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	case WM_DESTROY:
	{
		NetManager::Instance()->close_connect();
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
