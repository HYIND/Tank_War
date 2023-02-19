#include "Scene.h"
#include "Network.h"
#include "keymap.h"

namespace Location
{
	RECT rect;
	int broder1;
	int broder2;
	int broder3;
	int broder4;
	int len_x;
	int len_y;
}
using namespace Location;

ID2D1SolidColorBrush* pWhite_Brush;
ID2D1SolidColorBrush* pBlack_Brush;

ID2D1SolidColorBrush* pRed_Brush;
ID2D1SolidColorBrush* pGreen_Brush;

ID2D1SolidColorBrush* pHall_Brush;
ID2D1SolidColorBrush* pHall_ClickBrush;

ID2D1SolidColorBrush* pMain_Brush;
ID2D1SolidColorBrush* pMain_ClickBrush;

IDWriteTextFormat* pMain_Format;
IDWriteTextFormat* pHall_Format;
IDWriteTextFormat* pPing_Format;
HFONT edit_listbox_front;

ID2D1Bitmap* OP_pBitmap;
ID2D1Bitmap* TEXT_pBitmap;

D2D1_RECT_F DelayRect;	//	延迟显示位

Scene* CurScene;

Scene* SMain;
Scene_Hall* SHall;
Scene_Option* SOption;
Scene_Room* SRoom_host;
Scene_Room* SRoom_nothost;
Scene* SGaming_local;
Scene* SGaming_online;
Scene* SEndGame;
Scene* SWinGame;
Scene* SFailGame;
Scene* SPause;

HWND userid_in;

HWND Scene_Room::Room_user_list;
HWND Scene_Room::Room_edit_in;
HWND Scene_Room::edit_room;

STATUS status = STATUS::Main;

int MoveX, MoveY, ClickX, ClickY;

LONG_PTR Key_map_PreProc;
/* 修改键位输入框回调函数 */
LRESULT CALLBACK Key_map_Proc1(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN: //对键盘消息处理
	{
		WCHAR wchar[10] = { L'\0' };
		GetKeyNameText(lParam, wchar, 10);
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)L"");
		SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM) & (wchar[0]));
		keybroad key_class = SOption->key_map_set1[hwnd];
		key_map_p1[key_class] = wParam;
		break;
	}
	}
	return CallWindowProc((WNDPROC)Key_map_PreProc, hwnd, message, wParam, lParam);//其他事件交由父窗口处理
}
/* 修改键位输入框回调函数 */
LRESULT CALLBACK Key_map_Proc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN: //对键盘消息处理
	{
		WCHAR wchar[10] = { L'\0' };
		GetKeyNameText(lParam, wchar, 10);
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)L"");
		SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM) & (wchar[0]));
		keybroad key_class = SOption->key_map_set2[hwnd];
		key_map_p2[key_class] = wParam;
		break;
	}
	}
	return CallWindowProc((WNDPROC)Key_map_PreProc, hwnd, message, wParam, lParam);//其他事件交由父窗口处理
}

LONG_PTR Edit_Hall_PreProc;
/* 编辑框回调函数，实现 Crlt+回车 快捷键发送消息 */
LRESULT CALLBACK Edit_Hall_Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN: //对键盘消息处理
		if (wParam == VK_RETURN && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			SendMessage(_hwnd, WM_COMMAND, IDB_HALL_SEND, (LPARAM)_hwnd);
		}
		break;
	}
	return CallWindowProc((WNDPROC)Edit_Hall_PreProc, hwnd, message, wParam, lParam);//其他事件交由父窗口处理
}

/* 编辑框回调函数，实现 Crlt+回车 快捷键发送消息 */
LONG_PTR Edit_Room_PreProc;
LRESULT CALLBACK Edit_Room_Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN: //对键盘消息处理
		if (wParam == VK_RETURN && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			SendMessage(_hwnd, WM_COMMAND, IDB_ROOM_SEND, (LPARAM)_hwnd);
		}
		break;
	}
	return CallWindowProc((WNDPROC)Edit_Room_PreProc, hwnd, message, wParam, lParam);//其他事件交由父窗口处理
}

/* 暂停窗口回调函数 */
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

/* 登陆前设置ID窗口回调函数 */
BOOL CALLBACK GetID_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		userid_in = GetDlgItem(hDlg, Userid_in);
		SetWindowTextW(userid_in, my_userid.c_str());
		break;
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

void InitScene(ID2D1Factory*& pD2DFactory, ID2D1HwndRenderTarget*& pRenderTarget, IWICImagingFactory*& pIWICFactory, IDWriteFactory*& pDWriteFactory)
{
	try {
		::SMain = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SHall = new Scene_Hall(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SOption = new Scene_Option(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SRoom_host = new Scene_Room(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SRoom_nothost = new Scene_Room(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SGaming_local = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SGaming_online = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SWinGame = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SFailGame = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SPause = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
	}
	catch (exception& e)
	{
		return;
	}
}

void Load_SMain(RECT& rect)
{
	int loc1 = (rect.left + rect.right) / 2 - 120;
	int loc3 = (rect.left + rect.right) / 2 + 120;
	SMain->LoadButton(loc1, 80, loc3, 130,
		IDB_LOCALGAME,
		SMain->LoadText(loc1, 80, loc3, 130, L"开始游戏"));

	SMain->LoadButton(loc1, 180, loc3, 230,
		IDB_ENTERHALL,
		SMain->LoadText(loc1, 180, loc3, 230, L"联机大厅"));

	SMain->LoadButton(loc1, 280, loc3, 330,
		IDB_OPTION,
		SMain->LoadText(loc1, 280, loc3, 330, L"设置"));

	SMain->LoadButton(loc1, 380, loc3, 430,
		IDB_QUITGAME,
		SMain->LoadText(loc1, 380, loc3, 430, L"退出游戏"));
}
void Load_SHall(RECT& rect)
{
	SHall->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);

	SHall->LoadResourceBitmap(broder1 + len_x * 6 + 10, broder2 + len_y * 5, broder3 - 10, broder4 - len_y, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.2f);

	{
		SHall->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
			IDB_EXITHALL,
			SHall->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
	}

	{
		SHall->LoadText(broder1 + len_x, broder2,
			broder1 + len_x * 4, broder2 + len_y - 3,
			L"房间列表", pHall_Brush, pHall_Brush, pHall_Format);
		SHall->LoadText(broder1 + len_x * 7 - 5, broder2 + 5,
			broder1 + len_x * 9 + 5, broder2 + len_y + 3,
			L"大厅用户列表", pHall_Brush, pHall_Brush, pHall_Format);

		SHall->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 5.5,
			broder1 + len_x, broder2 + len_y * 6,
			L"公", pWhite_Brush, pWhite_Brush, pHall_Format);
		SHall->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 6,
			broder1 + len_x, broder2 + len_y * 6.5,
			L"共", pWhite_Brush, pWhite_Brush, pHall_Format);
		SHall->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 6.5,
			broder1 + len_x, broder2 + len_y * 7,
			L"频", pWhite_Brush, pWhite_Brush, pHall_Format);
		SHall->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 7,
			broder1 + len_x, broder2 + len_y * 7.5,
			L"道", pWhite_Brush, pWhite_Brush, pHall_Format);
	}

	{
		SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
			IDB_ENTERROOM,
			SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
				L"加入房间", pHall_Brush, pHall_ClickBrush, pHall_Format));

		SHall->LoadButton(broder1 + len_x * 8, broder2 + len_y * 5 + 10, broder3 - 10, broder2 + len_y * 6,
			IDB_CREATEROOM,
			SHall->LoadText(broder1 + len_x * 8, broder2 + len_y * 5 + 10, broder3 - 10, broder2 + len_y * 6,
				L"创建房间", pHall_Brush, pHall_ClickBrush, pHall_Format));

		SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 6 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 7,
			IDB_REFRESH,
			SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 6 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 7,
				L"刷新", pHall_Brush, pHall_ClickBrush, pHall_Format));

		SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
			IDB_HALL_SEND,
			SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
				L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));
	}

	{
		SHall->Hall_room_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			broder1 + len_x, broder2 + len_y - 5,
			len_x * 4, len_y * 4,
			_hwnd, (HMENU)HALL_ROOM_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		SHall->Hall_user_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			broder1 + len_x * 7, broder2 + len_y + 5,
			len_x * 2, len_y * 3,
			_hwnd, (HMENU)HALL_USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		SHall->edit_hall = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
			broder1 + len_x, broder2 + len_y * 5,
			len_x * 5, len_y * 3 - 10,
			_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		SHall->Hall_edit_in = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
			broder1 + len_x, broder2 + len_y * 8,
			len_x * 5, len_y * 2 - 10,
			_hwnd, (HMENU)HALL_EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);

		Edit_Hall_PreProc = SetWindowLongPtr(SHall->Hall_edit_in, GWLP_WNDPROC, (LONG_PTR)Edit_Hall_Proc);

		/* 设置字体 */
		SendMessage(
			SHall->Hall_room_list,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			SHall->Hall_user_list,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			SHall->edit_hall,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			SHall->Hall_edit_in,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
	}
}
void Load_Soption(RECT& rect)
{
	SOption->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);
	SOption->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
		IDB_EXITOPTION,
		SOption->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));

	{		/* 帧率设置 */
		SOption->LoadText(broder1 + len_x * 2, broder2 + len_y,
			broder1 + len_x * 4, broder2 + len_y * 1.5,
			L"设置", pWhite_Brush, pWhite_Brush, pHall_Format);

		SOption->LoadText(broder1 + len_x * 1, broder2 + len_y * 2,
			broder1 + len_x * 2, broder2 + len_y * 3,
			L"帧率", pWhite_Brush, pWhite_Brush, pHall_Format);
		SOption->LoadText(broder1 + len_x * 3 + 10, broder2 + len_y * 2,
			broder1 + len_x * 4, broder2 + len_y * 3,
			L"30", pWhite_Brush, pWhite_Brush, pPing_Format);
		SOption->LoadText(broder1 + len_x * 5 + 10, broder2 + len_y * 2,
			broder1 + len_x * 6, broder2 + len_y * 3,
			L"60", pWhite_Brush, pWhite_Brush, pPing_Format);
		SOption->LoadText(broder1 + len_x * 7 + 10, broder2 + len_y * 2,
			broder1 + len_x * 8, broder2 + len_y * 3,
			L"144", pWhite_Brush, pWhite_Brush, pPing_Format);
		SOption->LoadButton(broder1 + len_x * 3 - 12, broder2 + len_y * 2.5 - 12,
			broder1 + len_x * 3 + 12, broder2 + len_y * 2.5 + 12,
			IDB_SETFPS_30);
		SOption->LoadButton(broder1 + len_x * 5 - 12, broder2 + len_y * 2.5 - 12,
			broder1 + len_x * 5 + 12, broder2 + len_y * 2.5 + 12,
			IDB_SETFPS_60);
		SOption->LoadButton(broder1 + len_x * 7 - 12, broder2 + len_y * 2.5 - 12,
			broder1 + len_x * 7 + 12, broder2 + len_y * 2.5 + 12,
			IDB_SETFPS_144);
	}

	{	/* 键位设置 */
		SOption->LoadText(broder1 + len_x * 2, broder2 + len_y * 4,
			broder1 + len_x * 4, broder2 + len_y * 5,
			L"键位设置", pWhite_Brush, pHall_Brush, pHall_Format);

		{	/* P1 */
			SOption->LoadText(broder1 + len_x * 1, broder2 + len_y * 5,
				broder1 + len_x * 2, broder2 + len_y * 7.5,
				L"P1", pWhite_Brush, pWhite_Brush, pPing_Format);

			SOption->LoadText(broder1 + len_x * 2, broder2 + len_y * 5,
				broder1 + len_x * 2.5, broder2 + len_y * 5.5,
				L"UP", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key1_UP = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 2.5, broder2 + len_y * 5.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_UP, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key1_UP, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			SOption->LoadText(broder1 + len_x * 2, broder2 + len_y * 5.5,
				broder1 + len_x * 2.5, broder2 + len_y * 6,
				L"DOWN", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key1_DOWN = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 2.5, broder2 + len_y * 5.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_DOWN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key1_DOWN, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			SOption->LoadText(broder1 + len_x * 2, broder2 + len_y * 6,
				broder1 + len_x * 2.5, broder2 + len_y * 6.5,
				L"LEFT", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key1_LEFT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 2.5, broder2 + len_y * 6.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_LEFT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key1_LEFT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			SOption->LoadText(broder1 + len_x * 2, broder2 + len_y * 6.5,
				broder1 + len_x * 2.5, broder2 + len_y * 7,
				L"RIGHT", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key1_RIGHT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 2.5, broder2 + len_y * 6.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key1_RIGHT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			SOption->LoadText(broder1 + len_x * 2, broder2 + len_y * 7,
				broder1 + len_x * 2.5, broder2 + len_y * 7.5,
				L"FIRE", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key1_FIRE = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 2.5, broder2 + len_y * 7.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key1_FIRE, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);
		}

		{	/* P2 */
			SOption->LoadText(broder1 + len_x * 4, broder2 + len_y * 5,
				broder1 + len_x * 5, broder2 + len_y * 7.5,
				L"P2", pWhite_Brush, pWhite_Brush, pPing_Format);

			SOption->LoadText(broder1 + len_x * 5, broder2 + len_y * 5,
				broder1 + len_x * 5.5, broder2 + len_y * 5.5,
				L"UP", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key2_UP = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 5.5, broder2 + len_y * 5.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_UP, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key2_UP, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			SOption->LoadText(broder1 + len_x * 5, broder2 + len_y * 5.5,
				broder1 + len_x * 5.5, broder2 + len_y * 6,
				L"DOWN", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key2_DOWN = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 5.5, broder2 + len_y * 5.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_DOWN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key2_DOWN, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			SOption->LoadText(broder1 + len_x * 5, broder2 + len_y * 6,
				broder1 + len_x * 5.5, broder2 + len_y * 6.5,
				L"LEFT", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key2_LEFT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 5.5, broder2 + len_y * 6.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_LEFT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key2_LEFT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			SOption->LoadText(broder1 + len_x * 5, broder2 + len_y * 6.5,
				broder1 + len_x * 5.5, broder2 + len_y * 7,
				L"RIGHT", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key2_RIGHT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 5.5, broder2 + len_y * 6.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key2_RIGHT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			SOption->LoadText(broder1 + len_x * 5, broder2 + len_y * 7,
				broder1 + len_x * 5.5, broder2 + len_y * 7.5,
				L"FIRE", pWhite_Brush, pWhite_Brush, pPing_Format);
			SOption->key2_FIRE = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder1 + len_x * 5.5, broder2 + len_y * 7.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(SOption->key2_FIRE, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);
		}

		{	/* 键位绑定 */
			SOption->HWND_Messager.emplace_back(&SOption->key1_UP);
			SOption->HWND_Messager.emplace_back(&SOption->key1_DOWN);
			SOption->HWND_Messager.emplace_back(&SOption->key1_LEFT);
			SOption->HWND_Messager.emplace_back(&SOption->key1_RIGHT);
			SOption->HWND_Messager.emplace_back(&SOption->key1_FIRE);

			SOption->HWND_Messager.emplace_back(&SOption->key2_UP);
			SOption->HWND_Messager.emplace_back(&SOption->key2_DOWN);
			SOption->HWND_Messager.emplace_back(&SOption->key2_LEFT);
			SOption->HWND_Messager.emplace_back(&SOption->key2_RIGHT);
			SOption->HWND_Messager.emplace_back(&SOption->key2_FIRE);

			SOption->key_map_set1[SOption->key1_UP] = keybroad::UP;
			SOption->key_map_set1[SOption->key1_DOWN] = keybroad::DOWN;
			SOption->key_map_set1[SOption->key1_LEFT] = keybroad::LEFT;
			SOption->key_map_set1[SOption->key1_RIGHT] = keybroad::RIGHT;
			SOption->key_map_set1[SOption->key1_FIRE] = keybroad::FIRE;

			SOption->key_map_set2[SOption->key2_UP] = keybroad::UP;
			SOption->key_map_set2[SOption->key2_DOWN] = keybroad::DOWN;
			SOption->key_map_set2[SOption->key2_LEFT] = keybroad::LEFT;
			SOption->key_map_set2[SOption->key2_RIGHT] = keybroad::RIGHT;
			SOption->key_map_set2[SOption->key2_FIRE] = keybroad::FIRE;
		}
	}
	/* 设置字体 */
	for (auto& v : SOption->HWND_Messager)
	{
		SendMessage(
			*v,  //设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
	}
	SOption->Get_Key();
}
void Load_SRoom(RECT& rect)
{
	{
		SRoom_host->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);
		SRoom_host->LoadResourceBitmap(broder1 + len_x * 6 + 10, broder2 + len_y * 5, broder3 - 10, broder4 - len_y, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.2f);
		{
			SRoom_host->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
				IDB_EXITROOM,
				SRoom_host->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
		}

		{
			SRoom_host->LoadText(broder1 + len_x, broder2,
				broder1 + len_x * 4, broder2 + len_y - 3,
				L"当前房间内玩家情况", pHall_Brush, pHall_Brush, pHall_Format);

			SRoom_host->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 5.5,
				broder1 + len_x, broder2 + len_y * 6,
				L"房", pWhite_Brush, pWhite_Brush, pHall_Format);
			SRoom_host->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 6,
				broder1 + len_x, broder2 + len_y * 6.5,
				L"间", pWhite_Brush, pWhite_Brush, pHall_Format);
			SRoom_host->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 6.5,
				broder1 + len_x, broder2 + len_y * 7,
				L"频", pWhite_Brush, pWhite_Brush, pHall_Format);
			SRoom_host->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 7,
				broder1 + len_x, broder2 + len_y * 7.5,
				L"道", pWhite_Brush, pWhite_Brush, pHall_Format);
		}

		{
			SRoom_host->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
				IDB_STARTGAME,
				SRoom_host->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
					L"开始游戏", pHall_Brush, pHall_ClickBrush, pHall_Format));

			SRoom_host->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
				IDB_ROOM_SEND,
				SRoom_host->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
					L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));
		}
	}


	{
		SRoom_nothost->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);
		SRoom_nothost->LoadResourceBitmap(broder1 + len_x * 6 + 10, broder2 + len_y * 5, broder3 - 10, broder4 - len_y, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.5f);
		{
			SRoom_nothost->LoadText(broder1 + len_x, broder2,
				broder1 + len_x * 4, broder2 + len_y - 3,
				L"当前房间内玩家情况", pHall_Brush, pHall_Brush, pHall_Format);

			SRoom_nothost->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 5.5,
				broder1 + len_x, broder2 + len_y * 6,
				L"房", pWhite_Brush, pWhite_Brush, pHall_Format);
			SRoom_nothost->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 6,
				broder1 + len_x, broder2 + len_y * 6.5,
				L"间", pWhite_Brush, pWhite_Brush, pHall_Format);
			SRoom_nothost->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 6.5,
				broder1 + len_x, broder2 + len_y * 7,
				L"频", pWhite_Brush, pWhite_Brush, pHall_Format);
			SRoom_nothost->LoadText(broder1 + len_x * 0.5, broder2 + len_y * 7,
				broder1 + len_x, broder2 + len_y * 7.5,
				L"道", pWhite_Brush, pWhite_Brush, pHall_Format);
		}
		{
			SRoom_nothost->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
				IDB_EXITROOM,
				SRoom_nothost->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
		}

		{
			SRoom_nothost->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
				IDB_READY,
				SRoom_nothost->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
					L"准备", pHall_Brush, pHall_ClickBrush, pHall_Format));

			SRoom_nothost->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
				IDB_ROOM_SEND,
				SRoom_nothost->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
					L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));
		}
	}


	{

		Scene_Room::Room_user_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			broder1 + len_x, broder2 + len_y - 5,
			len_x * 4, len_y * 4,
			_hwnd, (HMENU)ROOM_USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Scene_Room::edit_room = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
			broder1 + len_x, broder2 + len_y * 5,
			len_x * 5, len_y * 3 - 10,
			_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Scene_Room::Room_edit_in = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
			broder1 + len_x, broder2 + len_y * 8,
			len_x * 5, len_y * 2 - 10,
			_hwnd, (HMENU)ROOM_EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);

		Edit_Room_PreProc = SetWindowLongPtr(Scene_Room::Room_edit_in, GWLP_WNDPROC, (LONG_PTR)Edit_Room_Proc);//这里很重要

		/* 设置字体 */
		SendMessage(
			Scene_Room::Room_user_list,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			Scene_Room::edit_room,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			Scene_Room::Room_edit_in,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
	}
}
void Load_SGaming(RECT& rect)
{
	{
		SGaming_local->LoadButton(rect.left, rect.top, rect.left + 100, rect.top + 67,
			IDB_PAUSE,
			SGaming_local->LoadResourceBitmap(rect.left, rect.top, rect.left + 100, rect.top + 67, L"PNG", MAKEINTRESOURCE(PAUSE_PNG)));
	}


	{
		SGaming_online->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
			IDB_RETURN,
			SGaming_online->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
	}
}
void Load_SWinGame(RECT& rect)
{
	int middle = rect.left + (rect.right - rect.left) / 2;
	try
	{
		::SWinGame->LoadButton(middle - 100, 250, middle + 100, 350,
			ReturnInEndGame,
			::SWinGame->LoadText(middle - 100, 250, middle + 100, 350, L"返回"));
		::SWinGame->LoadResourceBitmap(middle - 120, 30, middle + 120, 230, L"PNG", MAKEINTRESOURCE(WIN_PNG));

	}
	catch (std::exception& e)
	{
		return;
	}
}
void Load_SFailGame(RECT& rect)
{
	int middle = rect.left + (rect.right - rect.left) / 2;
	try
	{
		::SFailGame->LoadButton(middle - 100, 250, middle + 100, 350,
			ReturnInEndGame,
			::SFailGame->LoadText(middle - 100, 250, middle + 100, 350, L"返回"));
		::SFailGame->LoadResourceBitmap(middle - 100, 100, middle + 100, 190, L"PNG", MAKEINTRESOURCE(FAIL_PNG));

	}
	catch (std::exception& e)
	{
		return;
	}
}

void Load_D2DUI(RECT& rect)
{
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(OPBK_PNG), &OP_pBitmap);
	Load_SMain(rect);
	Load_SHall(rect);
	Load_Soption(rect);
	Load_SRoom(rect);
	//Load_EndGame(rect);
	Load_SWinGame(rect);
	Load_SFailGame(rect);
	Load_SGaming(rect);
}

void Init_D2DTool(RECT& rect)
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	HRESULT hr = S_OK;

	hr = pD2DFactory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(_hwnd, SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&pRenderTarget
	);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pWhite_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pBlack_Brush);

	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 0, 0, 1), &pRed_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 1, 0, 1), &pGreen_Brush);

	pMain_Brush = pBlack_Brush;
	pMain_ClickBrush = pWhite_Brush;
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pMain_Brush);
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pMain_ClickBrush);

	pHall_Brush = pBlack_Brush;
	pHall_ClickBrush = pWhite_Brush;
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pHall_Brush);
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pHall_ClickBrush);


	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pIDWriteFactory)
	);

	hr = pIDWriteFactory->CreateTextFormat(
		L"SimSun",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		50.0f,                         // Size    
		L"zh-cn",                      // Local
		&pMain_Format                 // Pointer to recieve the created object
	);
	hr = pMain_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = pMain_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	hr = pIDWriteFactory->CreateTextFormat(
		L"Gabriola",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		25.0f,                         // Size    
		L"zh-cn",                      // Local
		&pHall_Format                 // Pointer to recieve the created object
	);

	hr = pIDWriteFactory->CreateTextFormat(
		L"SimHei",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		20.0f,                         // Size    
		L"zh-cn",                      // Local
		&pPing_Format                 // Pointer to recieve the created object
	);

	hr = pHall_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = pHall_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	hr = pPing_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = pPing_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	edit_listbox_front = CreateFont(
		-16/*高度*/, -8/*宽度*/, 0/*不用管*/, 0/*不用管*/, 500 /*一般这个值设为400*/,
		FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
		DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
		DEFAULT_QUALITY,  //默认输出质量
		FF_DONTCARE,  //不指定字体族*/
		L"微软雅黑"  //字体名
	);

	DelayRect = RectF(rect.right - 60, rect.top + 5, rect.right - 5, rect.top + 30);
}

void Init_SceneResource()
{
	GetClientRect(_hwnd, &rect);

	broder1 = rect.left + 70;
	broder2 = rect.top + 30;
	broder3 = rect.right - 70;
	broder4 = rect.bottom - 30;
	len_x = (broder3 - broder1) / 10;
	len_y = (broder4 - broder2) / 10;

	Init_D2DTool(rect);
	InitScene(pD2DFactory, pRenderTarget, pIWICFactory, pIDWriteFactory);
	Load_D2DUI(rect);
}

void Scene_Option::DrawScene()
{
	Scene::DrawScene();

	//FPS选项
	{
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 3, broder2 + len_y * 2.5), 10, 10), pWhite_Brush);
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 5, broder2 + len_y * 2.5), 10, 10), pWhite_Brush);
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 7, broder2 + len_y * 2.5), 10, 10), pWhite_Brush);

		if (Fps == 30.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 3, broder2 + len_y * 2.5), 8, 8), pBlack_Brush);
		}
		else if (Fps == 60.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 5, broder2 + len_y * 2.5), 8, 8), pBlack_Brush);
		}
		else if (Fps == 144.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 7, broder2 + len_y * 2.5), 8, 8), pBlack_Brush);
		}
	}
}

void Scene_Option::Get_Key()
{
	for (auto& v : key_map_set1)
	{
		keybroad key_class = v.second;
		int VK_key = key_map_p1[key_class];
		WCHAR wchar[10] = { L'\0' };
		GetKeyNameText(MapVirtualKey(VK_key, 0) << 16, wchar, 10);
		SendMessage(v.first, WM_SETTEXT, 0, (LPARAM)L"");
		SendMessage(v.first, EM_REPLACESEL, FALSE, (LPARAM) & (wchar[0]));
	}
	for (auto& v : key_map_set2)
	{
		keybroad key_class = v.second;
		int VK_key = key_map_p2[key_class];
		WCHAR wchar[10] = { L'\0' };
		GetKeyNameText(MapVirtualKey(VK_key, 0) << 16, wchar, 10);
		SendMessage(v.first, WM_SETTEXT, 0, (LPARAM)L"");
		SendMessage(v.first, EM_REPLACESEL, FALSE, (LPARAM) & (wchar[0]));
	}
}

void Show_Hall(bool flag)
{
	if (flag)
	{
		(int)SendMessage(SHall->Hall_room_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(SHall->Hall_user_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(SHall->edit_hall, WM_SETTEXT, 0, (LPARAM)L"");
		(int)SendMessage(SHall->Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
		ShowWindow(SHall->edit_hall, SW_SHOW);
		ShowWindow(SHall->Hall_edit_in, SW_SHOW);
		ShowWindow(SHall->Hall_room_list, SW_SHOW);
		ShowWindow(SHall->Hall_user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(SHall->edit_hall, SW_HIDE);
		ShowWindow(SHall->Hall_edit_in, SW_HIDE);
		ShowWindow(SHall->Hall_room_list, SW_HIDE);
		ShowWindow(SHall->Hall_user_list, SW_HIDE);
	}
}
void Show_Room(bool flag)
{
	if (flag)
	{
		(int)SendMessage(Scene_Room::Room_user_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(Scene_Room::edit_room, WM_SETTEXT, 0, (LPARAM)L"");
		(int)SendMessage(Scene_Room::Room_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
		ShowWindow(Scene_Room::edit_room, SW_SHOW);
		ShowWindow(Scene_Room::Room_edit_in, SW_SHOW);
		ShowWindow(Scene_Room::Room_user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(Scene_Room::edit_room, SW_HIDE);
		ShowWindow(Scene_Room::Room_edit_in, SW_HIDE);
		ShowWindow(Scene_Room::Room_user_list, SW_HIDE);
	}
}
void Show_Option(bool flag)
{
	if (flag)
	{
		for (auto& v : SOption->HWND_Messager)
		{
			ShowWindow(*v, SW_SHOW);
		}
	}
	else
	{
		for (auto& v : SOption->HWND_Messager)
		{
			ShowWindow(*v, SW_HIDE);
		}
	}
}

void Set_CurScene(STATUS status_in)
{
	Show_Hall(false);
	Show_Room(false);
	Show_Option(false);
	switch (status_in)
	{
	case STATUS::Room_Status:
	{
		isonline_game = false;
		isstart = false;
		status = STATUS::Room_Status;
		if (!host)
		{
			if (isready)
			{
				isready = false;
				SRoom_nothost->ModifyButton_ID(IDB_CANCELREADY, IDB_READY);
				SRoom_nothost->ModifyText_byButton(IDB_READY, L"准备");
			}
			CurScene = SRoom_nothost;
		}
		else
		{
			CurScene = SRoom_host;
		}
		Show_Room(TRUE);
		Get_Room_Info();
		break;
	}
	case STATUS::Main:
	{
		isonline_game = false;
		isready = false;
		isstart = false;
		host = false;
		status = STATUS::Main;
		CurScene = SMain;
		break;
	}
	case STATUS::Option:
	{
		status = STATUS::Option;
		CurScene = SOption;
		Show_Option(TRUE);
		break;
	}
	case STATUS::Hall_Status:
	{
		isonline_game = false;
		isready = false;
		isstart = false;
		host = false;
		status = STATUS::Hall_Status;
		CurScene = SHall;
		Show_Hall(true);
		Get_Hallinfo();
		break;
	}
	case STATUS::Game_Status:
	{
		isstart = true;
		status = STATUS::Game_Status;
		if (isonline_game)
			CurScene = SGaming_online;
		else CurScene = SGaming_local;
	}
	default:
		break;
	}
}
