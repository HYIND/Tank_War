// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "WindowsProject1.h"
#include "Method.h"
#include "Scene.h"
#include "Tank.h"
//#include <GdiPlus.h>
//#pragma comment(lib,"Gdiplus.lib")
//using namespace Gdiplus;
//#include <comdef.h>

static HBRUSH text_brush = CreateSolidBrush(RGB(230, 230, 230));

using namespace D2D1;
//using namespace std;

#define MAX_LOADSTRING 100

//extern vector <tank_info*> tank_list;

ID2D1HwndRenderTarget* pRenderTarget_t;

// 全局变量:
Tank tank1(60, 60);
Tank tank2(60, 60);
Tank* mytank = &tank1;
Tank* optank = &tank2;

Tank* my_in = &tank1;
Tank* op_in = &tank2;

//ID2D1SolidColorBrush* pBrush = NULL; // A black brush, reflect the line color

LPCTSTR OP_Resource = L"C:\\Users\\H\\Desktop\\WindowsProject1\\x64\\Debug\\Resource\\OP_BK.jpg";
LPCTSTR Tank_Resource = L"C:\\Users\\H\\Desktop\\WindowsProject1\\x64\\Debug\\Resource\\Tank.bmp";
LPCTSTR TEXT_Resource = L"C:\\Users\\H\\Desktop\\WindowsProject1\\x64\\Debug\\Resource\\TEXT_BK.png";

HRESULT hr = S_OK;

ID2D1Bitmap* OP_pBitmap;
ID2D1Bitmap* TEXT_pBitmap;
ID2D1Bitmap* Tank_pBitmap;

HDC hdcmem, hdc, tdc;
bool isstart = false;
bool host = false;
bool isonline_game = false;
bool Hall_IOCP_flag = false;
bool Game_IOCP_flag = false;
bool reverse_in = true;

int status = NONE;

HANDLE Hall_hIOCP;
PPER_IO_DATA Hall_pPerIO;
HANDLE Game_hIOCP;
PPER_IO_DATA Game_pPerIO;

//加载位图
HBITMAP hbitmap1 = (HBITMAP)LoadImage(NULL, L"Resource\\OP_BK.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
//HBITMAP hbitmap2 = (HBITMAP)LoadImage(NULL, L"beila2.bmp", IMAGE_BITMAP, optank->width, optank->height, LR_DEFAULTSIZE | LR_LOADFROMFILE);
HBITMAP op_bitmap = (HBITMAP)LoadImage(NULL, L"Resource\\OP_BK.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
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
HWND room_list;
HWND user_list;
HWND edit_in;
HWND edit_hall;

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
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_CLIPCHILDREN | WS_EX_COMPOSITED | WS_EX_LAYERED,
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
enum { _tank, _bullet, online, refrash, reconnect, ping };	//定时器ID
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

		mytank->InitTank(rect.left + mytank->width / 2 + 20, (rect.bottom - rect.top) / 2, RIGHT);
		optank->InitTank(rect.right - optank->width / 2 - 20, (rect.bottom - rect.top) / 2, LEFT);
		//Get_Init_UI(_hwnd);
		Get_Initinfo();

		//GetObject(hbitmap1, sizeof(BITMAP), &bm1);
		////GetObject(hbitmap2, sizeof(BITMAP), &bm2);
		//GetObject(op_bitmap, sizeof(BITMAP), &op_bm);

		InitResource();
		hr = Loadbitmap(pIWICFactory, pRenderTarget, OP_Resource, &OP_pBitmap);
		hr = Loadbitmap(pIWICFactory, pRenderTarget, Tank_Resource, &Tank_pBitmap);
		if (FAILED(hr))
		{
			MessageBox(hWnd, _T("位图加载失败"), L"Error", MB_OK);
		}
		DelayRect = RectF(rect.right - 35, rect.top + 5, rect.right - 5, rect.top + 30);
		//{
		//	CoInitialize(NULL);
		//	CoCreateInstance(
		//		CLSID_WICImagingFactory,
		//		NULL,
		//		CLSCTX_INPROC_SERVER,
		//		IID_PPV_ARGS(&pIWICFactory)
		//	);
		//}

		CurScene = SMain;

		//RECT rect_t;
		//GetClientRect(edit_in,&rect_t);
		//	hr = pD2DFactory->CreateHwndRenderTarget(
		//		RenderTargetProperties(),
		//		HwndRenderTargetProperties(edit_in, SizeU(rect_t.right - rect_t.left, rect_t.bottom - rect_t.top)),
		//		&pRenderTarget_t
		//	);
		//	hr = Loadbitmap(pIWICFactory, pRenderTarget_t, TEXT_Resource, &TEXT_pBitmap);

		//StaticWndProc = (WNDPROC)::SetWindowLong(room_list, GWLP_WNDPROC, (LPARAM)MyStaticWndProc);
		//SetWindowLong(room_list, GWL_STYLE, GetWindowLong(room_list, GWL_STYLE) | WS_EX_TRANSPARENT);
		//SendMessage(room_list, LB_ADDSTRING, 0, L"1213213"[0]);
		//SendMessage(room_list, LB_ADDSTRING, 0, L"1213213"[0]);
		//SendMessage(room_list, LB_ADDSTRING, 0, L"1213213"[0]);
		//SendMessage(room_list, LB_ADDSTRING, 0, L"1213213"[0]);
		//room_list = CreateWindow(L"EDIT", L"",
		//	WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER | WS_TABSTOP | LBS_NOTIFY |OWNERDRA,//如果不设定LBS_HASSTRINGS，那么GetText取得的将是乱码
		//	20, 300, 360, 440, hWnd, (HMENU)IDC_LIST1, GetModuleHandle(nullptr), nullptr);
		break;
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
			break;
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
	//case WM_ACTIVATE:
	//{
	//}
	//case WM_MEASUREITEM://ODT_LISTBOX
	//	if ((UINT)wParam == IDC_LIST2)
	//	{
	//		LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
	//		lpmis->itemWidth = 400;
	//		lpmis->itemHeight = 30;
	//	}
	//	break;
	//case WM_DRAWITEM:
	//{
	//	if ((UINT)wParam == IDC_LIST2)
	//	{
	//		LPDRAWITEMSTRUCT pDI = (LPDRAWITEMSTRUCT)lParam;
	//		HBRUSH brsh = CreateSolidBrush(RGB(0, 0, 0));
	//		RECT rect;
	//		GetClientRect(room_list, &rect);
	//		FillRect(pDI->hDC, &rect, brsh);
	//		DeleteObject(brsh);
	//		// text 
	//		SetBkMode(pDI->hDC, TRANSPARENT);
	//		//wchar_t szText[260];
	//		//SendMessage(room_list, LB_GETTEXT, pDI->itemID, (LPARAM)szText);
	//		//const DWORD dwStyle = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS;
	//		//DrawText(pDI->hDC, szText, wcslen(szText), &pDI->rcItem, dwStyle);
	//	}
	//	break;

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
		switch (status)
		{
		case NONE:
		{
			switch (wmId)
			{
				// 开始游戏
			case IDB_TWO: {
				Init_all();
				//Hide_Main_UI();
				//ShowWindow(hwndButton1, SW_SHOW);
				SetTimer(hWnd, _tank, 50, NULL);
				SetTimer(hWnd, _bullet, 50, NULL);
				CurScene = SGaming;
				isstart = true;
				status = Game_Status;
				isonline_game = false;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
						// 联机大厅
			case IDB_THREE: {
				if (!Init_Hall())
					break;
				//Hide_Main_UI();
				Show_Hall(true);
				CurScene = SHall;
				status = Hall_Status;
				SetTimer(hWnd, reconnect, 5000, NULL);
				SetTimer(hWnd, refrash, 3000, NULL);
				SetTimer(hWnd, ping, 3000, NULL);
				UpdateWindow(hWnd);
				break;
			}


						  // 设置
			case IDB_FOUR: {break; }
			case IDB_FIVE:// 退出游戏
			{
				DestroyWindow(hWnd);
				break;
			}
			}
			break;
		}
		case Hall_Status:
		{
			switch (wmId)
			{
			case Enterroom:
			{
				status = Room_Status;
				//ShowWindow(Hall, SW_HIDE);
				//Room = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_ROOM), _hwnd, ROOM);
				//ShowWindow(Room, SW_SHOW);
				CurScene = SRoom;
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
				int index = SendMessage(room_list, LB_GETCURSEL, 0, 0);
				//TCHAR buff[255];
				//SendMessage(room_list,LB_GETTEXT, index, (LPARAM)buff);
				//wstring wtemp = string2wstring(to_string(index));
				//MessageBox(hDlg, buff, &wtemp[0], MB_OK);
				Enter_Room(index);
				break;
			}
			// 离开大厅
			case IDB_EXITHALL:
				closesocket(mysocket);
				Show_Hall(false);
				CurScene = SMain;
				//Show_Main_UI();
				KillTimer(hWnd, reconnect);
				KillTimer(hWnd, refrash);
				KillTimer(hWnd, ping);
				status = NONE;
				UpdateWindow(_hwnd);
				break;
			case IDB_SEND:
			{
				wstring temp;
				GetWindowTextW(edit_in, &(temp[0]), 1024);
				Send_Message(temp);
				break;
			}
			// 创建房间
			case IDB_CREATEROOM:
			{
				Create_Room();
				//Room = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_ROOM), _hwnd, ROOM);
				//ShowWindow(Hall, SW_HIDE);
				//ShowWindow(Room, SW_SHOW);
				CurScene = SRoom;
				host = true;
				status = Room_Status;
				UpdateWindow(_hwnd);
				break;
			}
			}
			break;
		}
		case Room_Status:
		{
			break;
		}
		case Game_Status:
		{
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

			case WIN:
			{
				CurScene = SWinGame;
				break;
			}

			case ReturnInEndGame:
			{
				isstart = false;
				isonline_game = true;
				ShowWindow(Room, SW_SHOW);
				CurScene = SRoom;
				break;
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
				//SafeRelease(pBrush);
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

	case WM_KEYDOWN:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		switch (wParam)
		{
		case VK_SPACE:
		{
			if (isstart)
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
		}
		case VK_OEM_2:
		{
			if (isstart)
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
		}
		//InvalidateRect(hWnd, NULL, TRUE);
		}
	}
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLOREDIT:
	{
		HWND t_hwnd = (HWND)lParam;
		if (t_hwnd == edit_in || t_hwnd == edit_hall || t_hwnd == room_list || t_hwnd == user_list)
		{
			//RECT text_rect;
			//GetClientRect(t_hwnd, &text_rect);
			//pRenderTarget_t->BeginDraw();
			//pRenderTarget_t->Clear(ColorF(1, 1, 1, 1));
			//pRenderTarget_t->DrawBitmap(TEXT_pBitmap, D2D1::RectF(text_rect.left, text_rect.top, text_rect.right, text_rect.bottom),0.4f);
			//pRenderTarget_t->EndDraw();
			HDC t_hdc = (HDC)wParam;
			SetTextColor(t_hdc, RGB(0, 0, 0));   //设置文字颜色
			SetBkColor(t_hdc, RGB(230, 230, 230));//设置文字背景颜色
			SetBkMode(t_hdc, TRANSPARENT);;
			////RECT rect;
			////GetClientRect(hWnd, &rect);
			////HDC m_hdc = GetDC(hWnd);
			//////通过SetStretchBltMode的设置能使StretchBlt在缩放图像更加清晰
			////SetStretchBltMode((HDC)wParam, COLORONCOLOR);
			////StretchBlt((HDC)wParam, 0, 0, rect.right, rect.bottom, hdcmem, 0, 0, bm1.bmWidth, bm1.bmHeight, SRCCOPY);
			return (INT_PTR)text_brush;
		}
		break;
	}
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
	case WM_PAINT:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		{
			pRenderTarget->BeginDraw();
			pRenderTarget->Clear(ColorF(1, 1, 1, 1));
			if (status != Game_Status) {
				pRenderTarget->DrawBitmap(OP_pBitmap, D2D1::RectF(0, 0, rect.right, rect.bottom));

				if (FAILED(hr))
				{
					MessageBox(NULL, L"Draw failed!", L"Error", 0);
				}
			}
			if (CurScene)
				CurScene->DrawScene();
			if (status != NONE || (status == Game_Status && isonline_game))
			{
				wstring ws = to_wstring(delay);
				const wchar_t* delay_ch = ws.c_str();
				pRenderTarget->DrawText(
					delay_ch,
					wcslen(delay_ch),
					pPing_Format,
					DelayRect,
					pMain_Brush
				);
			}
			if (status == Game_Status)
			{
				if (tank1.bullet_head)
					(*(tank1.bullet_head)).Drawbullet(pRenderTarget, NULL);
				if (tank2.bullet_head)
					(*(tank2.bullet_head)).Drawbullet(pRenderTarget, NULL);

				if (tank1.isalive)
					tank1.DrawTank(pRenderTarget, Tank_pBitmap);
				if (tank2.isalive)
					tank2.DrawTank(pRenderTarget, Tank_pBitmap);
			}
			hr = pRenderTarget->EndDraw();
		}
		EndPaint(hWnd, &ps);
		// 
		// 
		//RECT rc;
		//GetClientRect(hWnd, &rc);

		//// Create a Direct2D render target          
		//ID2D1HwndRenderTarget* pRT = NULL;
		//HRESULT hr = pD2DFactory->CreateHwndRenderTarget(
		//	D2D1::RenderTargetProperties(),
		//	D2D1::HwndRenderTargetProperties(
		//		hWnd,
		//		D2D1::SizeU(
		//			rc.right - rc.left,
		//			rc.bottom - rc.top)
		//	),
		//	&pRT
		//);

		//ID2D1SolidColorBrush* pBrush = NULL;
		//if (SUCCEEDED(hr))
		//{

		//	pRT->CreateSolidColorBrush(
		//		D2D1::ColorF(D2D1::ColorF::Blue),
		//		&pBrush
		//	);
		//}
		//pRT->BeginDraw();
		//pRT->DrawRectangle(
		//	D2D1::RectF(
		//		rc.left + 100.0f,
		//		rc.top + 100.0f,
		//		rc.right - 100.0f,
		//		rc.bottom - 100.0f),
		//	pBrush);
		//pRT->CreateSolidColorBrush(
		//	D2D1::ColorF(D2D1::ColorF::White),
		//	&pBrush
		//);
		//pRT->FillRectangle(&RectF,);

		//hr = pRT->EndDraw();
		break;
	}

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

//bool edit_in_focus = false;
//INT_PTR CALLBACK HALL(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
//	UNREFERENCED_PARAMETER(lParam);
//	switch (message)
//	{
//	case WM_INITDIALOG:
//	{
//		status = Hall_Status;
//		SetTimer(hDlg, reconnect, 5000, NULL);
//		SetTimer(hDlg, refrash, 3000, NULL);
//		{
//			RECT Father_rect;
//			RECT My_rect;
//			GetClientRect(_hwnd, &Father_rect);
//			GetClientRect(hDlg, &My_rect);
//			SetWindowPos(
//				hDlg,
//				NULL,
//				Father_rect.left + (Father_rect.right - Father_rect.left) / 2 - My_rect.right / 2,
//				Father_rect.top + (Father_rect.bottom - Father_rect.top) / 2 - My_rect.bottom / 2,
//				100, 200,
//				SWP_NOSIZE
//			);
//		}
//		user_list = GetDlgItem(hDlg, IDC_LIST1);
//		room_list = GetDlgItem(hDlg, IDC_LIST2);
//		edit_hall = GetDlgItem(hDlg, IDC_EDIT1);
//		edit_in = GetDlgItem(hDlg, IDC_EDIT2);
//		{
//			WSAStartup(MAKEWORD(2, 2), &wsa);
//			ZeroMemory(&addr_info, sizeof(addr_info));
//			addr_info.sin_family = AF_INET;
//			addr_info.sin_port = htons(DEFAULT_PORT);
//			inet_pton(AF_INET, SERVER_IP, &(addr_info.sin_addr.S_un.S_addr));
//
//			memset(buffer, '\0', 1024);
//			mysocket = WSASocket(addr_info.sin_family, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//			connect(mysocket, (struct sockaddr*)&addr_info, sizeof(struct sockaddr));
//
//			Hall_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
//			CreateIoCompletionPort((HANDLE)mysocket, Hall_hIOCP, NULL, 0);
//			Hall_pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
//			Hall_pPerIO->nOperationType = OP_READ;
//			WSABUF buf;
//			buf.buf = Hall_pPerIO->buf;
//			buf.len = 1024;
//			DWORD dwRecv;
//			DWORD dwFlags = 0;
//			WSARecv(mysocket, &buf, 1, &dwRecv, &dwFlags, &Hall_pPerIO->ol, NULL);
//			thread T(Recv_Thread, Hall_pPerIO, LPVOID(Hall_hIOCP));
//			T.detach();
//			Get_Hallinfo();
//		}
//		//SetWindowLong(room_list, GWL_STYLE, GetWindowLong(room_list, GWL_STYLE) | LBS_OWNERDRAWFIXED);
//		break;
//	}
//
//	case WM_COMMAND:
//	{
//		int wmId = LOWORD(wParam);
//		switch (wParam)
//		{
//		case Enterroom:
//		{
//			status = Room_Status;
//			ShowWindow(Hall, SW_HIDE);
//			Room = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_ROOM), _hwnd, ROOM);
//			ShowWindow(Room, SW_SHOW);
//			UpdateWindow(_hwnd);
//			break;
//		}
//		// 刷新
//		case IDC_BUTTON1:
//		{
//			Get_Hallinfo();
//			break;
//		}
//		// 加入房间
//		case IDC_BUTTON2:
//		{
//			int index = SendMessage(room_list, LB_GETCURSEL, 0, 0);
//			//TCHAR buff[255];
//			//SendMessage(room_list,LB_GETTEXT, index, (LPARAM)buff);
//			//wstring wtemp = string2wstring(to_string(index));
//			//MessageBox(hDlg, buff, &wtemp[0], MB_OK);
//			Enter_Room(index);
//			break;
//		}
//		// 离开大厅
//		case IDC_BUTTON3:
//			closesocket(mysocket);
//			DestroyWindow(hDlg);
//			status = NONE;
//			Show_Main_UI();
//			UpdateWindow(_hwnd);
//			break;
//		case IDC_BUTTON4:
//		{
//			wstring temp;
//			GetWindowTextW(edit_in, &(temp[0]), 1024);
//			Send_Message(temp);
//			break;
//		}
//		// 创建房间
//		case IDC_BUTTON5:
//		{
//			ShowWindow(Hall, SW_HIDE);
//			Room = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_ROOM), _hwnd, ROOM);
//			host = true;
//			ShowWindow(Room, SW_SHOW);
//			UpdateWindow(_hwnd);
//			status = Room_Status;
//			Create_Room();
//			break;
//		}
//		if (HIWORD(wParam) == EN_SETFOCUS)
//		{
//			if ((HWND)lParam == edit_in) {
//				edit_in_focus = true;
//			}
//		}
//		if (HIWORD(wParam) == EN_KILLFOCUS)
//		{
//			if ((HWND)lParam == edit_in) {
//				edit_in_focus = true;
//			}
//		}
//		default: return DefWindowProc(hDlg, message, wParam, lParam);
//		}
//		break;
//	}
//	case WM_TIMER:
//	{
//		switch (wParam)
//		{
//		case refrash:
//			Get_Hallinfo();
//			break;
//			//case reconnect:
//			//if (!isconnecting())			
//			//	if (connect(mysocket, (struct sockaddr*)&addr_info, sizeof(struct sockaddr)) == -1)
//			//	{
//			//		(int)SendMessage(user_list, LB_RESETCONTENT, 0, 0);
//			//		(int)SendMessage(user_list, LB_ADDSTRING, 0, (LPARAM) L"已断开连接，检查网络！");
//			//	}
//			break;
//		}
//	}
//	//case WM_MEASUREITEM://ODT_LISTBOX
//	//	if ((UINT)wParam == IDC_LIST2)
//	//	{
//	//		LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
//	//		lpmis->itemWidth = 400;
//	//		lpmis->itemHeight = 22;
//	//	}
//	//	break;
//	//case WM_DRAWITEM:
//	//{
//	//	if ((UINT)wParam == IDC_LIST2)
//	//	{
//	//		LPDRAWITEMSTRUCT pDI = (LPDRAWITEMSTRUCT)lParam;
//	//		HBRUSH brsh = CreateSolidBrush(RGB(255 - 30 * pDI->itemID, 128 + 40 * pDI->itemID, 128 + 40 * pDI->itemID));//yellow
//	//		FillRect(pDI->hDC, &pDI->rcItem, brsh);
//	//		DeleteObject(brsh);
//	//		// text 
//	//		SetBkMode(pDI->hDC, TRANSPARENT);
//	//		wchar_t szText[260];
//	//		SendMessage(room_list, LB_GETTEXT, pDI->itemID, (LPARAM)szText);
//	//		const DWORD dwStyle = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS;
//	//		DrawText(pDI->hDC, szText, wcslen(szText), &pDI->rcItem, dwStyle);
//	//	}
//	//	break;
//
//	//}
//	case WM_KEYDOWN:
//	{
//		switch (wParam)
//		{
//		case VK_RETURN:
//			if (edit_in_focus) {
//				wstring temp;
//				GetWindowTextW(edit_in, &(temp[0]), 1024);
//				Send_Message(temp);
//				break;
//			}
//		}
//	}
//
//	case WM_CTLCOLOREDIT:
//	{
//		HWND t_hwnd = (HWND)lParam;
//		//if (t_hwnd == room_list)
//		//{
//		HDC t_hdc = (HDC)wParam;
//		SetTextColor(t_hdc, RGB(0, 0, 255));   //设置文字颜色
//		SetBkColor(t_hdc, RGB(200, 200, 200));//设置文字背景颜色
//		SetBkMode(t_hdc, TRANSPARENT);
//		//RECT text_rect;
//		//GetClientRect(t_hwnd, &text_rect);
//		//pRenderTarget_t->BeginDraw();
//		////RECT rect;
//		////GetClientRect(hWnd, &rect);
//		////HDC m_hdc = GetDC(hWnd);
//		//////通过SetStretchBltMode的设置能使StretchBlt在缩放图像更加清晰
//		////SetStretchBltMode((HDC)wParam, COLORONCOLOR);
//		////StretchBlt((HDC)wParam, 0, 0, rect.right, rect.bottom, hdcmem, 0, 0, bm1.bmWidth, bm1.bmHeight, SRCCOPY);
//		//pRenderTarget_t->Clear(ColorF(1, 1, 1, 1));
//		//pRenderTarget_t->DrawBitmap(TEXT_pBitmap, D2D1::RectF(text_rect.left, text_rect.top, text_rect.right, text_rect.bottom),0.4f);
//		//pRenderTarget_t->EndDraw();
//		return (INT_PTR)text_brush;
//		//}
//		break;
//	}
//	}
//	return (INT_PTR)FALSE;
//}

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
			status = Room_Status;
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
					My_rect.right - 270,         // starting x position
					My_rect.bottom - 70,         // starting y position
					80,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_FIVE,       // No menu
					(HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE),
					NULL);      // pointer not needed
			}
			{
				CreateWindow(
					L"BUTTON",	// predefined class    不区分大小写
					L"退出房间",       // button text
					WS_CHILD | WS_VISIBLE,  // styles     注意 如果样式写错了 Button 将不会正常显示

				   // Size and position values are given explicitly, because
				   // the CW_USEDEFAULT constant gives zero values for buttons.
					My_rect.right - 160,         // starting x position
					My_rect.bottom - 70,         // starting y position
					80,        // button width
					30,        // button height
					hDlg,       // parent window
					(HMENU)IDB_SIX,       // No menu
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
			send(mysocket, "StartGame", 1023, 0);
			break;
		}
		case IDB_SIX:
		{
			send(mysocket, "QuitRoom", 1023, 0);
			DestroyWindow(hDlg);
			status = Hall_Status;
			ShowWindow(Hall, SW_SHOW);
			UpdateWindow(_hwnd);
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
		case QUITROOM:
		{
			MessageBox(hDlg, L"房主已将房间解散！", NULL, MB_OK);
			DestroyWindow(hDlg);
			status = Hall_Status;
			ShowWindow(Hall, SW_SHOW);
			UpdateWindow(_hwnd);
			break;
		}
		}
	}
	}
	}
	return (INT_PTR)FALSE;
}