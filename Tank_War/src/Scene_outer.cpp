#include "Scene.h"
#include "Network.h"
#include "keymap.h"

namespace RectBroder
{
	RECT rect;
	int broder_left;
	int broder_top;
	int broder_right;
	int broder_bottom;
	int len_x;
	int len_y;
}
using namespace RectBroder;

namespace Brush
{

	ID2D1SolidColorBrush* pWhite_Brush;
	ID2D1SolidColorBrush* pBlack_Brush;

	ID2D1SolidColorBrush* Brush::pRed_Brush;
	ID2D1SolidColorBrush* pGreen_Brush;

	ID2D1SolidColorBrush* Brush::pHall_Brush;
	ID2D1SolidColorBrush* Brush::pHall_ClickBrush;

	ID2D1SolidColorBrush* Brush::pMain_Brush;
	ID2D1SolidColorBrush* Brush::pMain_ClickBrush;

}

namespace TextFormat
{
	IDWriteTextFormat* pMain_Format;
	IDWriteTextFormat* pHall_Format;
	IDWriteTextFormat* pPing_Format;

}

HFONT edit_listbox_front;

ID2D1Bitmap* OP_pBitmap;
ID2D1Bitmap* TEXT_pBitmap;

D2D1_RECT_F DelayRect;	//	延迟显示位


namespace _Scene
{
	vector<Scene*> Scene_list;

	Scene* _Scene::CurScene;

	Scene_Main* SMain;
	Scene_Hall* SHall;
	Scene_Option* SOption;
	Scene_Room_Host* SRoom_host;
	Scene_Room_NoHost* SRoom_nothost;
	Scene_Gaming_local* SGaming_local;
	Scene_Gaming_online* SGaming_online;
	Scene_WinGame* SWinGame;
	Scene_FailGame* SFailGame;
	Scene_Pause* SPause;
}
bool Scene_Room::isLoad = false;

HWND userid_in;

HWND Scene_Room::Room_user_list;
HWND Scene_Room::Room_edit_in;
HWND Scene_Room::edit_room;

STATUS status = STATUS::Main;

namespace MousePos
{
	int MoveX, MoveY, ClickX, ClickY;
}

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
		keybroad key_class = _Scene::SOption->key_map_set1[hwnd];
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
		keybroad key_class = _Scene::SOption->key_map_set2[hwnd];
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
		SetWindowTextW(userid_in, NetManager::Instance()->Get_my_userid().c_str());
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
			wstring userid = temp;
			NetManager::Instance()->Set_my_userid(userid);
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

void Scene_Main::Load(RECT& rect)
{
	int loc1 = (rect.left + rect.right) / 2 - 120;
	int loc3 = (rect.left + rect.right) / 2 + 120;
	LoadButton(loc1, 80, loc3, 130,
		IDB_LOCALGAME,
		LoadText(loc1, 80, loc3, 130, L"开始游戏"));

	LoadButton(loc1, 180, loc3, 230,
		IDB_ENTERHALL,
		LoadText(loc1, 180, loc3, 230, L"联机大厅"));

	LoadButton(loc1, 280, loc3, 330,
		IDB_OPTION,
		LoadText(loc1, 280, loc3, 330, L"设置"));

	LoadButton(loc1, 380, loc3, 430,
		IDB_QUITGAME,
		LoadText(loc1, 380, loc3, 430, L"退出游戏"));
}

void Scene_Hall::Load(RECT& rect)
{
	LoadResourceBitmap(broder_left, broder_top, broder_right, broder_bottom, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);

	LoadResourceBitmap(broder_left + len_x * 6 + 10, broder_top + len_y * 5, broder_right - 10, broder_bottom - len_y, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.2f);

	{
		LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
			IDB_EXITHALL,
			LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
	}

	{
		LoadText(broder_left + len_x, broder_top,
			broder_left + len_x * 4, broder_top + len_y - 3,
			L"房间列表", Brush::pHall_Brush, Brush::pHall_Brush, TextFormat::pHall_Format);
		LoadText(broder_left + len_x * 7 - 5, broder_top + 5,
			broder_left + len_x * 9 + 5, broder_top + len_y + 3,
			L"大厅用户列表", Brush::pHall_Brush, Brush::pHall_Brush, TextFormat::pHall_Format);

		LoadText(broder_left + len_x * 0.5, broder_top + len_y * 5.5,
			broder_left + len_x, broder_top + len_y * 6,
			L"公", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
		LoadText(broder_left + len_x * 0.5, broder_top + len_y * 6,
			broder_left + len_x, broder_top + len_y * 6.5,
			L"共", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
		LoadText(broder_left + len_x * 0.5, broder_top + len_y * 6.5,
			broder_left + len_x, broder_top + len_y * 7,
			L"频", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
		LoadText(broder_left + len_x * 0.5, broder_top + len_y * 7,
			broder_left + len_x, broder_top + len_y * 7.5,
			L"道", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
	}

	{
		LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 5 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 6,
			IDB_ENTERROOM,
			_Scene::SHall->LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 5 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 6,
				L"加入房间", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));

		LoadButton(broder_left + len_x * 8, broder_top + len_y * 5 + 10, broder_right - 10, broder_top + len_y * 6,
			IDB_CREATEROOM,
			_Scene::SHall->LoadText(broder_left + len_x * 8, broder_top + len_y * 5 + 10, broder_right - 10, broder_top + len_y * 6,
				L"创建房间", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));

		LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 6 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 7,
			IDB_REFRESH,
			_Scene::SHall->LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 6 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 7,
				L"刷新", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));

		LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 8 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 9,
			IDB_HALL_SEND,
			_Scene::SHall->LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 8 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 9,
				L"发送", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));
	}

	{
		Hall_room_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			broder_left + len_x, broder_top + len_y - 5,
			len_x * 4, len_y * 4,
			_hwnd, (HMENU)HALL_ROOM_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Hall_user_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			broder_left + len_x * 7, broder_top + len_y + 5,
			len_x * 2, len_y * 3,
			_hwnd, (HMENU)HALL_USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		edit_hall = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
			broder_left + len_x, broder_top + len_y * 5,
			len_x * 5, len_y * 3 - 10,
			_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Hall_edit_in = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
			broder_left + len_x, broder_top + len_y * 8,
			len_x * 5, len_y * 2 - 10,
			_hwnd, (HMENU)HALL_EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);

		Edit_Hall_PreProc = SetWindowLongPtr(Hall_edit_in, GWLP_WNDPROC, (LONG_PTR)Edit_Hall_Proc);

		/* 设置字体 */
		SendMessage(
			Hall_room_list,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			Hall_user_list,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			edit_hall,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			Hall_edit_in,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
	}
}

void Scene_Room::Load(RECT& rect)
{
	if (isLoad == false)
	{

		Room_user_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			broder_left + len_x, broder_top + len_y - 5,
			len_x * 4, len_y * 4,
			_hwnd, (HMENU)ROOM_USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		edit_room = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
			broder_left + len_x, broder_top + len_y * 5,
			len_x * 5, len_y * 3 - 10,
			_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Room_edit_in = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
			broder_left + len_x, broder_top + len_y * 8,
			len_x * 5, len_y * 2 - 10,
			_hwnd, (HMENU)ROOM_EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);

		Edit_Room_PreProc = SetWindowLongPtr(Scene_Room::Room_edit_in, GWLP_WNDPROC, (LONG_PTR)Edit_Room_Proc);	//设置回调

		/* 设置字体 */
		SendMessage(
			Room_user_list,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			edit_room,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
		SendMessage(
			Room_edit_in,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);

		isLoad = true;
	}
}

void Scene_Option::Load(RECT& rect)
{
	LoadResourceBitmap(broder_left, broder_top, broder_right, broder_bottom, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);
	LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
		IDB_EXITOPTION,
		LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));

	{		/* 帧率设置 */
		LoadText(broder_left + len_x * 2, broder_top + len_y,
			broder_left + len_x * 4, broder_top + len_y * 1.5,
			L"设置", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);

		LoadText(broder_left + len_x * 1, broder_top + len_y * 2,
			broder_left + len_x * 2, broder_top + len_y * 3,
			L"帧率", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
		LoadText(broder_left + len_x * 3 + 10, broder_top + len_y * 2,
			broder_left + len_x * 4, broder_top + len_y * 3,
			L"30", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
		LoadText(broder_left + len_x * 5 + 10, broder_top + len_y * 2,
			broder_left + len_x * 6, broder_top + len_y * 3,
			L"60", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
		LoadText(broder_left + len_x * 7 + 10, broder_top + len_y * 2,
			broder_left + len_x * 8, broder_top + len_y * 3,
			L"144", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
		LoadButton(broder_left + len_x * 3 - 12, broder_top + len_y * 2.5 - 12,
			broder_left + len_x * 3 + 12, broder_top + len_y * 2.5 + 12,
			IDB_SETFPS_30);
		LoadButton(broder_left + len_x * 5 - 12, broder_top + len_y * 2.5 - 12,
			broder_left + len_x * 5 + 12, broder_top + len_y * 2.5 + 12,
			IDB_SETFPS_60);
		LoadButton(broder_left + len_x * 7 - 12, broder_top + len_y * 2.5 - 12,
			broder_left + len_x * 7 + 12, broder_top + len_y * 2.5 + 12,
			IDB_SETFPS_144);
	}

	{	/* 键位设置 */
		LoadText(broder_left + len_x * 2, broder_top + len_y * 4,
			broder_left + len_x * 4, broder_top + len_y * 5,
			L"键位设置", Brush::pWhite_Brush, Brush::pHall_Brush, TextFormat::pHall_Format);

		{	/* P1 */
			LoadText(broder_left + len_x * 1, broder_top + len_y * 5,
				broder_left + len_x * 2, broder_top + len_y * 7.5,
				L"P1", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);

			LoadText(broder_left + len_x * 2, broder_top + len_y * 5,
				broder_left + len_x * 2.5, broder_top + len_y * 5.5,
				L"UP", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key1_UP = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 2.5, broder_top + len_y * 5.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_UP, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key1_UP, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			LoadText(broder_left + len_x * 2, broder_top + len_y * 5.5,
				broder_left + len_x * 2.5, broder_top + len_y * 6,
				L"DOWN", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key1_DOWN = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 2.5, broder_top + len_y * 5.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_DOWN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key1_DOWN, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			LoadText(broder_left + len_x * 2, broder_top + len_y * 6,
				broder_left + len_x * 2.5, broder_top + len_y * 6.5,
				L"LEFT", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key1_LEFT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 2.5, broder_top + len_y * 6.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_LEFT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key1_LEFT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			LoadText(broder_left + len_x * 2, broder_top + len_y * 6.5,
				broder_left + len_x * 2.5, broder_top + len_y * 7,
				L"RIGHT", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key1_RIGHT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 2.5, broder_top + len_y * 6.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key1_RIGHT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);

			LoadText(broder_left + len_x * 2, broder_top + len_y * 7,
				broder_left + len_x * 2.5, broder_top + len_y * 7.5,
				L"FIRE", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key1_FIRE = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 2.5, broder_top + len_y * 7.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key1_FIRE, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc1);
		}

		{	/* P2 */
			LoadText(broder_left + len_x * 4, broder_top + len_y * 5,
				broder_left + len_x * 5, broder_top + len_y * 7.5,
				L"P2", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);

			LoadText(broder_left + len_x * 5, broder_top + len_y * 5,
				broder_left + len_x * 5.5, broder_top + len_y * 5.5,
				L"UP", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key2_UP = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 5.5, broder_top + len_y * 5.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_UP, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key2_UP, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			LoadText(broder_left + len_x * 5, broder_top + len_y * 5.5,
				broder_left + len_x * 5.5, broder_top + len_y * 6,
				L"DOWN", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key2_DOWN = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 5.5, broder_top + len_y * 5.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_DOWN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key2_DOWN, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			LoadText(broder_left + len_x * 5, broder_top + len_y * 6,
				broder_left + len_x * 5.5, broder_top + len_y * 6.5,
				L"LEFT", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key2_LEFT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 5.5, broder_top + len_y * 6.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_LEFT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key2_LEFT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			LoadText(broder_left + len_x * 5, broder_top + len_y * 6.5,
				broder_left + len_x * 5.5, broder_top + len_y * 7,
				L"RIGHT", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key2_RIGHT = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 5.5, broder_top + len_y * 6.75 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key2_RIGHT, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);

			LoadText(broder_left + len_x * 5, broder_top + len_y * 7,
				broder_left + len_x * 5.5, broder_top + len_y * 7.5,
				L"FIRE", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pPing_Format);
			key2_FIRE = CreateWindowW(L"EDIT", L"",
				WS_CHILD | ES_CENTER | ES_READONLY,
				broder_left + len_x * 5.5, broder_top + len_y * 7.25 - 11,
				len_x * 1, 22,
				_hwnd, (HMENU)EDIT_EKY1_RIGHT, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
			Key_map_PreProc = SetWindowLongPtr(key2_FIRE, GWLP_WNDPROC, (LONG_PTR)Key_map_Proc2);
		}

		{	/* 键位绑定 */
			HWND_Messager.emplace_back(&key1_UP);
			HWND_Messager.emplace_back(&key1_DOWN);
			HWND_Messager.emplace_back(&key1_LEFT);
			HWND_Messager.emplace_back(&key1_RIGHT);
			HWND_Messager.emplace_back(&key1_FIRE);

			HWND_Messager.emplace_back(&key2_UP);
			HWND_Messager.emplace_back(&key2_DOWN);
			HWND_Messager.emplace_back(&key2_LEFT);
			HWND_Messager.emplace_back(&key2_RIGHT);
			HWND_Messager.emplace_back(&key2_FIRE);

			key_map_set1[key1_UP] = keybroad::UP;
			key_map_set1[key1_DOWN] = keybroad::DOWN;
			key_map_set1[key1_LEFT] = keybroad::LEFT;
			key_map_set1[key1_RIGHT] = keybroad::RIGHT;
			key_map_set1[key1_FIRE] = keybroad::FIRE;

			key_map_set2[key2_UP] = keybroad::UP;
			key_map_set2[key2_DOWN] = keybroad::DOWN;
			key_map_set2[key2_LEFT] = keybroad::LEFT;
			key_map_set2[key2_RIGHT] = keybroad::RIGHT;
			key_map_set2[key2_FIRE] = keybroad::FIRE;
		}
	}
	/* 设置字体 */
	for (auto& v : HWND_Messager)
	{
		SendMessage(
			*v,  //设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)edit_listbox_front,  //字体句柄
			NULL  //传空值即可
		);
	}
	Get_Key();
}

void Scene_Room_Host::Load(RECT& rect)
{
	Scene_Room::Load(rect);
	{
		LoadResourceBitmap(broder_left, broder_top, broder_right, broder_bottom, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);
		LoadResourceBitmap(broder_left + len_x * 6 + 10, broder_top + len_y * 5, broder_right - 10, broder_bottom - len_y, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.2f);
		{
			LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
				IDB_EXITROOM,
				LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
		}

		{
			LoadText(broder_left + len_x, broder_top,
				broder_left + len_x * 4, broder_top + len_y - 3,
				L"当前房间内玩家情况", Brush::pHall_Brush, Brush::pHall_Brush, TextFormat::pHall_Format);

			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 5.5,
				broder_left + len_x, broder_top + len_y * 6,
				L"房", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 6,
				broder_left + len_x, broder_top + len_y * 6.5,
				L"间", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 6.5,
				broder_left + len_x, broder_top + len_y * 7,
				L"频", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 7,
				broder_left + len_x, broder_top + len_y * 7.5,
				L"道", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
		}

		{
			LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 5 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 6,
				IDB_STARTGAME,
				LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 5 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 6,
					L"开始游戏", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));

			LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 8 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 9,
				IDB_ROOM_SEND,
				LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 8 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 9,
					L"发送", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));
		}
	}
}

void Scene_Room_NoHost::Load(RECT& rect)
{
	Scene_Room::Load(rect);
	{
		LoadResourceBitmap(broder_left, broder_top, broder_right, broder_bottom, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.4f);
		LoadResourceBitmap(broder_left + len_x * 6 + 10, broder_top + len_y * 5, broder_right - 10, broder_bottom - len_y, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.5f);
		{
			LoadText(broder_left + len_x, broder_top,
				broder_left + len_x * 4, broder_top + len_y - 3,
				L"当前房间内玩家情况", Brush::pHall_Brush, Brush::pHall_Brush, TextFormat::pHall_Format);

			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 5.5,
				broder_left + len_x, broder_top + len_y * 6,
				L"房", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 6,
				broder_left + len_x, broder_top + len_y * 6.5,
				L"间", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 6.5,
				broder_left + len_x, broder_top + len_y * 7,
				L"频", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
			LoadText(broder_left + len_x * 0.5, broder_top + len_y * 7,
				broder_left + len_x, broder_top + len_y * 7.5,
				L"道", Brush::pWhite_Brush, Brush::pWhite_Brush, TextFormat::pHall_Format);
		}
		{
			LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
				IDB_EXITROOM,
				LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
		}

		{
			LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 5 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 6,
				IDB_READY,
				LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 5 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 6,
					L"准备", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));

			LoadButton(broder_left + len_x * 6 + 10, broder_top + len_y * 8 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 9,
				IDB_ROOM_SEND,
				LoadText(broder_left + len_x * 6 + 10, broder_top + len_y * 8 + 10, broder_left + len_x * 8 - 10, broder_top + len_y * 9,
					L"发送", Brush::pHall_Brush, Brush::pHall_ClickBrush, TextFormat::pHall_Format));
		}
	}
}

void Scene_Gaming_local::Load(RECT& rect)
{

	LoadButton(rect.left, rect.top, rect.left + 100, rect.top + 67,
		IDB_PAUSE,
		LoadResourceBitmap(rect.left, rect.top, rect.left + 100, rect.top + 67, L"PNG", MAKEINTRESOURCE(PAUSE_PNG)));

}

void Scene_Gaming_online::Load(RECT& rect)
{
	LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
		IDB_RETURN,
		LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
}

void Scene_WinGame::Load(RECT& rect)
{
	int middle = rect.left + (rect.right - rect.left) / 2;
	try
	{
		LoadButton(middle - 100, 250, middle + 100, 350,
			ReturnInEndGame,
			LoadText(middle - 100, 250, middle + 100, 350, L"返回"));
		LoadResourceBitmap(middle - 120, 30, middle + 120, 230, L"PNG", MAKEINTRESOURCE(WIN_PNG));

	}
	catch (std::exception& e)
	{
		return;
	}
}

void Scene_FailGame::Load(RECT& rect)
{
	int middle = rect.left + (rect.right - rect.left) / 2;
	try
	{
		LoadButton(middle - 100, 250, middle + 100, 350,
			ReturnInEndGame,
			LoadText(middle - 100, 250, middle + 100, 350, L"返回"));
		LoadResourceBitmap(middle - 100, 100, middle + 100, 190, L"PNG", MAKEINTRESOURCE(FAIL_PNG));

	}
	catch (std::exception& e)
	{
		return;
	}
}

void Scene_Option::OnDrawScene()
{
	Scene::OnDrawScene();

	//FPS选项
	{
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder_left + len_x * 3, broder_top + len_y * 2.5), 10, 10), Brush::pWhite_Brush);
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder_left + len_x * 5, broder_top + len_y * 2.5), 10, 10), Brush::pWhite_Brush);
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder_left + len_x * 7, broder_top + len_y * 2.5), 10, 10), Brush::pWhite_Brush);

		if (Fps == 30.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder_left + len_x * 3, broder_top + len_y * 2.5), 8, 8), Brush::pBlack_Brush);
		}
		else if (Fps == 60.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder_left + len_x * 5, broder_top + len_y * 2.5), 8, 8), Brush::pBlack_Brush);
		}
		else if (Fps == 144.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder_left + len_x * 7, broder_top + len_y * 2.5), 8, 8), Brush::pBlack_Brush);
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

void Init_D2DTool(RECT& rect)
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	HRESULT hr = S_OK;

	hr = pD2DFactory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(_hwnd, SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&pRenderTarget
	);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &Brush::pWhite_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &Brush::pBlack_Brush);

	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 0, 0, 1), &Brush::pRed_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 1, 0, 1), &Brush::pGreen_Brush);

	Brush::pMain_Brush = Brush::pBlack_Brush;
	Brush::pMain_ClickBrush = Brush::pWhite_Brush;
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &Brush::pMain_Brush);
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &Brush::pMain_ClickBrush);

	Brush::pHall_Brush = Brush::pBlack_Brush;
	Brush::pHall_ClickBrush = Brush::pWhite_Brush;
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &Brush::pHall_Brush);
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &Brush::pHall_ClickBrush);


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
		&TextFormat::pMain_Format                 // Pointer to recieve the created object
	);
	hr = TextFormat::pMain_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = TextFormat::pMain_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	hr = pIDWriteFactory->CreateTextFormat(
		L"Gabriola",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		25.0f,                         // Size    
		L"zh-cn",                      // Local
		&TextFormat::pHall_Format                 // Pointer to recieve the created object
	);

	hr = pIDWriteFactory->CreateTextFormat(
		L"SimHei",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		20.0f,                         // Size    
		L"zh-cn",                      // Local
		&TextFormat::pPing_Format                 // Pointer to recieve the created object
	);

	hr = TextFormat::pHall_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = TextFormat::pHall_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	hr = TextFormat::pPing_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = TextFormat::pPing_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

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

void InitScene(ID2D1Factory*& pD2DFactory, ID2D1HwndRenderTarget*& pRenderTarget, IWICImagingFactory*& pIWICFactory, IDWriteFactory*& pDWriteFactory)
{
	try {
		::_Scene::SMain = new Scene_Main(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SHall = new Scene_Hall(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SOption = new Scene_Option(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SRoom_host = new Scene_Room_Host(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SRoom_nothost = new Scene_Room_NoHost(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SGaming_local = new Scene_Gaming_local(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SGaming_online = new Scene_Gaming_online(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SWinGame = new Scene_WinGame(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SFailGame = new Scene_FailGame(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::_Scene::SPause = new Scene_Pause(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);


		::_Scene::Scene_list.emplace_back(::_Scene::SMain);
		::_Scene::Scene_list.emplace_back(::_Scene::SHall);
		::_Scene::Scene_list.emplace_back(::_Scene::SOption);
		::_Scene::Scene_list.emplace_back(::_Scene::SRoom_host);
		::_Scene::Scene_list.emplace_back(::_Scene::SRoom_nothost);
		::_Scene::Scene_list.emplace_back(::_Scene::SGaming_local);
		::_Scene::Scene_list.emplace_back(::_Scene::SGaming_online);
		::_Scene::Scene_list.emplace_back(::_Scene::SWinGame);
		::_Scene::Scene_list.emplace_back(::_Scene::SFailGame);
		::_Scene::Scene_list.emplace_back(::_Scene::SPause);
	}
	catch (exception& e)
	{
		return;
	}
}

void Load_D2DUI(RECT& rect)
{
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(OPBK_PNG), &OP_pBitmap);

	for (auto v : _Scene::Scene_list)
		v->Load(rect);
}

void Init_SceneResource()
{
	GetClientRect(_hwnd, &rect);

	broder_left = rect.left + 70;
	broder_top = rect.top + 30;
	broder_right = rect.right - 70;
	broder_bottom = rect.bottom - 30;
	len_x = (broder_right - broder_left) / 10;
	len_y = (broder_bottom - broder_top) / 10;

	Init_D2DTool(rect);
	InitScene(pD2DFactory, pRenderTarget, pIWICFactory, pIDWriteFactory);
	Load_D2DUI(rect);
}

void Show_Hall(bool flag)
{
	if (flag)
	{
		(int)SendMessage(_Scene::SHall->Hall_room_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(_Scene::SHall->Hall_user_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(_Scene::SHall->edit_hall, WM_SETTEXT, 0, (LPARAM)L"");
		(int)SendMessage(_Scene::SHall->Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
		ShowWindow(_Scene::SHall->edit_hall, SW_SHOW);
		ShowWindow(_Scene::SHall->Hall_edit_in, SW_SHOW);
		ShowWindow(_Scene::SHall->Hall_room_list, SW_SHOW);
		ShowWindow(_Scene::SHall->Hall_user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(_Scene::SHall->edit_hall, SW_HIDE);
		ShowWindow(_Scene::SHall->Hall_edit_in, SW_HIDE);
		ShowWindow(_Scene::SHall->Hall_room_list, SW_HIDE);
		ShowWindow(_Scene::SHall->Hall_user_list, SW_HIDE);
	}
}
void Scene_Room::Show_Room(bool flag)
{
	if (flag)
	{
		(int)SendMessage(Room_user_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(edit_room, WM_SETTEXT, 0, (LPARAM)L"");
		(int)SendMessage(Room_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
		ShowWindow(edit_room, SW_SHOW);
		ShowWindow(Room_edit_in, SW_SHOW);
		ShowWindow(Room_user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(edit_room, SW_HIDE);
		ShowWindow(Room_edit_in, SW_HIDE);
		ShowWindow(Room_user_list, SW_HIDE);
	}
}
void Show_Option(bool flag)
{
	if (flag)
	{
		for (auto& v : _Scene::SOption->HWND_Messager)
		{
			ShowWindow(*v, SW_SHOW);
		}
	}
	else
	{
		for (auto& v : _Scene::SOption->HWND_Messager)
		{
			ShowWindow(*v, SW_HIDE);
		}
	}
}

void Set_CurScene(STATUS status_in)
{
	Show_Hall(false);
	Scene_Room::Show_Room(false);
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
				_Scene::SRoom_nothost->ModifyButton_ID(IDB_CANCELREADY, IDB_READY);
				_Scene::SRoom_nothost->ModifyText_byButton(IDB_READY, L"准备");
			}
			_Scene::CurScene = _Scene::SRoom_nothost;
		}
		else
		{
			_Scene::CurScene = _Scene::SRoom_host;
		}
		Scene_Room::Show_Room(TRUE);
		NetManager::Instance()->Get_Room_Info();
		break;
	}
	case STATUS::Main:
	{
		isonline_game = false;
		isready = false;
		isstart = false;
		host = false;
		status = STATUS::Main;
		_Scene::CurScene = _Scene::SMain;
		break;
	}
	case STATUS::Option:
	{
		status = STATUS::Option;
		_Scene::CurScene = _Scene::SOption;
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
		_Scene::CurScene = _Scene::SHall;
		Show_Hall(true);
		NetManager::Instance()->Get_Hallinfo();
		break;
	}
	case STATUS::Game_Status:
	{
		isstart = true;
		status = STATUS::Game_Status;
		if (isonline_game)
			_Scene::CurScene = _Scene::SGaming_online;
		else _Scene::CurScene = _Scene::SGaming_local;
	}
	default:
		break;
	}
}
