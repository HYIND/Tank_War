// WindowsProject1.cpp : 定义应用程序的入口点。
//
#include "header.h"
#include "Render.h"
#include "Scene.h"
#include "Manager/GameWorldManager.h"
#include "Manager/RenderManager.h"
#include "Manager/ConnectManager.h"
#include "Manager/RequestManager.h"
#include "Manager/LobbyManager.h"


#define MAX_LOADSTRING 100

//#define AITEST
// 全局变量:

bool reverse_in = false;

HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING] = L"Tank War";                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND _hwnd;
RECT _rect;

bool Init_all_Resource()
{
	LOGINFO("Init", "Init_All");
	ResFactory->InitResource();
	Init_Scene();
	InitStyle();
	return true;
}


#define ShowConsole
// 创建并重定向控制台
bool CreateDebugConsole(const wchar_t* title = L"TankWar Debug Console") {
	// 1. 检查是否已有关联的控制台
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		// 已经附加到父进程的控制台（比如从命令行启动）
		printf("Attached to parent console\n");
	}
	else {
		// 2. 创建新的控制台
		if (!AllocConsole()) {
			DWORD err = GetLastError();
			if (err == ERROR_ACCESS_DENIED) {
				// 已经有关联的控制台
				printf("Console already allocated\n");
			}
			else {
				printf("Failed to allocate console: %lu\n", err);
				return false;
			}
		}
	}

	// 3. 设置控制台标题
	SetConsoleTitleW(title);

	// 4. 获取标准输出句柄
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOutput == INVALID_HANDLE_VALUE) {
		printf("Failed to get stdout handle\n");
		return false;
	}

	// 5. 获取控制台屏幕缓冲区信息
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOutput, &csbi);

	// 6. 设置控制台窗口大小和缓冲区大小
	SMALL_RECT rect = { 0, 0, 120, 900 };  // 宽度120字符，高度900行
	SetConsoleWindowInfo(hOutput, TRUE, &rect);

	COORD size = { 120, 1000 };  // 缓冲区比窗口稍大，支持滚动
	SetConsoleScreenBufferSize(hOutput, size);

	// 7. 关键步骤：重定向 C 运行时标准输入输出
	// 保存原来的标准输出
	int originalStdout = _dup(_fileno(stdout));
	int originalStderr = _dup(_fileno(stderr));
	int originalStdin = _dup(_fileno(stdin));

	// 重定向到控制台
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);

	// 8. 同步 C++ 标准流
	std::ios::sync_with_stdio();

	// 9. 清除缓冲区并测试输出
	std::cout.clear();
	std::cin.clear();
	std::cerr.clear();
	std::clog.clear();

	// 10. 设置控制台编码（支持中文）
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	// 11. 测试输出
	printf("========================================\n");
	printf("        Debug Console Started\n");
	printf("========================================\n");
	printf("This is a test message\n");
	printf("中文测试 Chinese Test\n");
	printf("========================================\n");
	fflush(stdout);

	return true;
}

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Pause(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Return(HWND, UINT, WPARAM, LPARAM);
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

enum { hall_refreash, room_refreash, sceneTick };	//定时器ID
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	GetClientRect(hWnd, &_rect);
	_hwnd = hWnd;
	switch (message)
	{
	case WM_CREATE:
	{
#ifdef ShowConsole
		CreateDebugConsole();
#endif // ShowConsole
		bool result = Init_all_Resource();
		if (!result)
		{
			MessageBox(hWnd, _T("位图加载失败"), L"Error", MB_OK);
			DestroyWindow(hWnd);
		}
		_Scene::CurScene = _Scene::SMain;
		{
			RenderManager::Instance()->getRenderer()->SetRenderTarget(pRenderTarget);
			thread T(&Render_Thread,
				_hwnd,
				RenderManager::Instance()->getRenderer(),
				RenderManager::Instance()->getfpsController(),
				RenderManager::Instance()->getBufferManager()
			);
			T.detach();
		}
		SetTimer(hWnd, sceneTick, 20, NULL);
		break;
	}

	case WM_TIMER:
	{
		switch (wParam)
		{
		case hall_refreash:
			REQUESTMANAGER->RequestLobbyUserData();
			REQUESTMANAGER->RequestLobbyRoomData();
			break;
		case room_refreash:
			REQUESTMANAGER->RequestRoomInfo();
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
		if (Get_CurScene() == STATUS::LocalGame_Status)
		{
			//KillTimer(hWnd, _game);
		}
		break;
	}

	case WM_SETFOCUS:
	{
		if (Get_CurScene() == STATUS::LocalGame_Status)
		{
			//SetTimer(hWnd, _game, 20, NULL);
		}
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (Get_CurScene())
		{
		case STATUS::Main:
		{
			switch (wmId)
			{
				// 开始游戏
			case IDB_LOCALGAME: {
				GameWorldManager::Instance()->InitGameWorld(GameMode::RunGame, 0);
				GameWorldManager::Instance()->RunWorld();
				Set_CurScene(STATUS::LocalGame_Status);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
							  // 联机大厅
			case IDB_ENTERHALL: {
				if (!DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_Userid), hWnd, (DLGPROC)GetID_Proc))
					break;
				if (!ConnectManager::Instance()->Login())
					break;
				Set_CurScene(STATUS::Hall_Status);
				SetTimer(hWnd, hall_refreash, 3000, NULL);
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
				//Set_CurScene(STATUS::MapEdit);
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
				RenderManager::Instance()->getfpsController()->setGameTargetFps(30);
				break;
			}
			case IDB_SETFPS_60:
			{
				RenderManager::Instance()->getfpsController()->setGameTargetFps(60);
				break;
			}
			case IDB_SETFPS_144:
			{
				RenderManager::Instance()->getfpsController()->setGameTargetFps(144);
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
				REQUESTMANAGER->RequestLobbyUserData();
				REQUESTMANAGER->RequestLobbyRoomData();
				break;
			}
			// 加入房间
			case IDB_ENTERROOM:
			{
				if (LOBBYMANAGER->TryJoinRoom())
				{
					SendMessage(_hwnd, WM_COMMAND, Enterroom, (LPARAM)_hwnd);
				}
				else
				{
					MessageBox(_hwnd, L"加入房间请求异常！", NULL, MB_OK);
				}
				break;
			}
			// 离开大厅
			case IDB_EXITHALL:
				ConnectManager::Instance()->Logout();
				KillTimer(hWnd, hall_refreash);
				Set_CurScene(STATUS::Main);
				UpdateWindow(_hwnd);
				break;
			case IDB_HALL_SEND:
			{
				wchar_t temp[1024] = { '\0' };
				GetWindowTextW(_Scene::SHall->Hall_edit_in, temp, 1024);
				wstring str = temp;
				if (!str.empty())
				{
					REQUESTMANAGER->SendHallMsg(Tool::WStringToUTF8(str));
					SendMessage(_Scene::SHall->Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
				}
				break;
			}
			// 创建房间
			case IDB_CREATEROOM:
			{
				if (LOBBYMANAGER->TryCreateRoom())
				{
					REQUESTMANAGER->RequestRoomInfo();
					Set_CurScene(STATUS::Room_Status);
					KillTimer(hWnd, hall_refreash);
					SetTimer(hWnd, room_refreash, 1000, NULL);
					UpdateWindow(_hwnd);
				}
				else
				{
					MessageBox(_hwnd, L"加入房间请求异常！", NULL, MB_OK);
				}
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
				if (!LOBBYMANAGER->IsHost())
					break;
				LOBBYMANAGER->TryStartGame();
				break;
			}
			case IDB_READY:
			{
				if (LOBBYMANAGER->TryChangeReadyStatus(true))
				{
					REQUESTMANAGER->RequestRoomInfo();
					_Scene::SRoom->ModifyButton_ID(IDB_READY, IDB_CANCELREADY);
					_Scene::SRoom->ModifyText_byButton(IDB_CANCELREADY, L"取消准备");
				}
				break;
			}
			case IDB_CANCELREADY:
			{
				if (LOBBYMANAGER->TryChangeReadyStatus(false))
				{
					REQUESTMANAGER->RequestRoomInfo();
					_Scene::SRoom->ModifyButton_ID(IDB_CANCELREADY, IDB_READY);
					_Scene::SRoom->ModifyText_byButton(IDB_READY, L"准备");
				}
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
				if (LOBBYMANAGER->TryLeaveRoom())
				{
					KillTimer(hWnd, room_refreash);
					Set_CurScene(STATUS::Hall_Status);
					SetTimer(hWnd, hall_refreash, 4000, NULL);
					UpdateWindow(hWnd);
				}
				break;
			}
			case START:
			{
				KillTimer(hWnd, room_refreash);

				GameWorldManager::Instance()->InitOnlineGameWorld();
				GameWorldManager::Instance()->RunWorld();
				Set_CurScene(STATUS::OnlineGame_Status);

				break;
			}
			case IDB_ROOM_SEND:
			{
				wchar_t temp[1024] = { '\0' };
				GetWindowTextW(_Scene::SRoom->Room_edit_in, temp, 1024);
				wstring str = temp;
				if (!str.empty())
				{
					REQUESTMANAGER->SendRoomMsg(Tool::WStringToUTF8(str));
					SendMessage(_Scene::SRoom->Room_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
				}
				break;
			}
			}
			break;
		}
		case STATUS::LocalGame_Status:
		{
			switch (wmId)
			{
				// 暂停
			case IDB_PAUSE:
			{
				//KillTimer(hWnd, _game);
				int i = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PAUSE), hWnd, Pause);
				switch (i) {
				case 10:	//回到游戏
					//SetTimer(hWnd, _game, 20, NULL);
					break;
				case 20:	//重新开始
					GameWorldManager::Instance()->StopWorld();
					GameWorldManager::Instance()->InitGameWorld(GameMode::RunGame, 1);
					GameWorldManager::Instance()->RunWorld();
					//SetTimer(hWnd, _game, 20, NULL);
					break;
				case 30:	//回到主菜单
					GameWorldManager::Instance()->StopWorld();
					Set_CurScene(STATUS::Main);
					break;
				}
				break;
			}
			case ReLoadMap:
			{
				//Game::Instance()->LoadLocalMap();
				break;
			}
			}
			break;
		}
		case STATUS::OnlineGame_Status:
		{
			switch (wmId)
			{
			case IDB_RETURN:
			{
				int i = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_RETURN), hWnd, Return);
				switch (i) {
				case 10:	//回到游戏
					break;
				case 20:	//退出游戏
					if (REQUESTMANAGER->RequestLeaveGame())
					{
						GameWorldManager::Instance()->StopWorld();
						Set_CurScene(STATUS::Hall_Status);
						SetTimer(hWnd, hall_refreash, 4000, NULL);
					}
					break;
					//case 30:	//回到主菜单
					//	GameWorldManager::Instance()->StopWorld();
					//	Set_CurScene(STATUS::Main);
					//	break;
				}
				break;
			}
			case WIN:
			{
				CONNECTMANAGER->LogoutGameSeervice();
				_Scene::CurScene = _Scene::SWinGame;
				break;
			}
			case FAIL:
			{
				CONNECTMANAGER->LogoutGameSeervice();
				_Scene::CurScene = _Scene::SFailGame;
				break;
			}
			case ReturnInEndGame:
			{
				SetTimer(hWnd, room_refreash, 1000, NULL);
				Set_CurScene(STATUS::Room_Status);
				_Scene::SRoom->Show(true);
				break;
			}
			}
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
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
