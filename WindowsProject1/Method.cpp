#include"Method.h"

extern bool isstart;
extern HWND hwndButton1;
extern HWND hwndButton2;
extern HWND hwndButton3;
extern HWND hwndButton4;
extern HWND hwndButton5;
extern HWND hwndButton6;
extern HWND _hwnd;
extern HDC hdc;

extern SOCKET mysocket;
extern char buffer[1024];

void START(HWND hWnd) {}

wstring string2wstring(string str)
{
	LPCSTR pszSrc = str.c_str();
	int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
	if (nLen == 0)
		return std::wstring(L"");

	wchar_t* pwszDst = new wchar_t[nLen];
	if (!pwszDst)
		return std::wstring(L"");

	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
	std::wstring wstr(pwszDst);
	delete[] pwszDst;
	pwszDst = NULL;

	return wstr;
}

string wstring2string(wstring wstr)
{
	LPCWSTR pwszSrc = wstr.c_str();
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (!pszDst)
		return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	std::string str(pszDst);
	delete[] pszDst;
	pszDst = NULL;

	return str;
}

void Get_Init_UI(HWND hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);

	{
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
			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
			NULL);      // pointer not needed
		ShowWindow(hwndButton1, SW_HIDE);
	}
	{
		hwndButton2 = CreateWindow(
			L"BUTTON",	// predefined class    不区分大小写
			L"本地游戏",       // button text
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

		   // Size and position values are given explicitly, because
		   // the CW_USEDEFAULT constant gives zero values for buttons.
			(rect.left + rect.right) / 2 - 90,         // starting x position
			80,         // starting y position
			180,        // button width
			50,        // button height
			hWnd,       // parent window
			(HMENU)IDB_TWO,       // No menu
			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
			NULL);      // pointer not needed
	}
	{
		hwndButton3 = CreateWindow(
			L"BUTTON",	// predefined class    不区分大小写
			L"联机大厅",       // button text
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

		   // Size and position values are given explicitly, because
		   // the CW_USEDEFAULT constant gives zero values for buttons.
			(rect.left + rect.right) / 2 - 90,         // starting x position
			180,         // starting y position
			180,        // button width
			50,        // button height
			hWnd,       // parent window
			(HMENU)IDB_THREE,       // No menu
			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
			NULL);      // pointer not needed
	}
	{
		hwndButton4 = CreateWindow(
			L"BUTTON",	// predefined class    不区分大小写
			L"设置",       // button text
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

		   // Size and position values are given explicitly, because
		   // the CW_USEDEFAULT constant gives zero values for buttons.
			(rect.left + rect.right) / 2 - 90,         // starting x position
			280,         // starting y position
			180,        // button width
			50,        // button height
			hWnd,       // parent window
			(HMENU)IDB_FOUR,       // No menu
			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
			NULL);      // pointer not needed
	}
	{
		hwndButton5 = CreateWindow(
			L"BUTTON",	// predefined class    不区分大小写
			L"退出游戏",       // button text
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示

		   // Size and position values are given explicitly, because
		   // the CW_USEDEFAULT constant gives zero values for buttons.
			(rect.left + rect.right) / 2 - 90,         // starting x position
			380,         // starting y position
			180,        // button width
			50,        // button height
			hWnd,       // parent window
			(HMENU)IDB_FIVE,       // No menu
			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
			NULL);      // pointer not needed
	}

}

void Show_Main_UI() {
	ShowWindow(hwndButton2, SW_SHOW);
	ShowWindow(hwndButton3, SW_SHOW);
	ShowWindow(hwndButton4, SW_SHOW);
	ShowWindow(hwndButton5, SW_SHOW);
}

void Hide_Main_UI() {
	ShowWindow(hwndButton2, SW_HIDE);
	ShowWindow(hwndButton3, SW_HIDE);
	ShowWindow(hwndButton4, SW_HIDE);
	ShowWindow(hwndButton5, SW_HIDE);
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
	ShowWindow(hwndButton5, SW_SHOW);

}

void SIG_IO(int sig) {

}

bool isconnecting() {
	int i = recv(mysocket, buffer, 1023, MSG_PEEK);
	if (i == 0)return false;
	else if (i == -1) {
		return (errno == EINTR);
	}
	return true;
}

string send_socket(string& s) {
	const char* send_buf = &(s[0]);
	send(mysocket, send_buf, 1023, 0);
	memset(buffer, '\0', 1024);
	int num = recv(mysocket, buffer, 1023, MSG_PEEK);
	if (num > 0) {
		recv(mysocket, buffer, 1023, 0);
		string re = buffer;
		memset(buffer, '\0', 1024);
		return re;
	}
	if (num == 0)return	"error";
}

void recv_socket() {
	HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort((HANDLE)mysocket, hIOCP, NULL, 0);
	DWORD dwBytesTranfered = 0;
	ULONG_PTR uKey;
	LPOVERLAPPED pov = NULL;
	GetQueuedCompletionStatus(
		hIOCP,
		&dwBytesTranfered,
		NULL,
		&pov,
		INFINITE
	);
	switch (pov->hEvent)
	{
		//接收新的连接
	case IO_ACCEPT:
		//连接完成后，再次投递一个连接的请求
		PostAccept(sockServer, hIocp);
		cout << " 有新的连接接入" << endl;
		PostRecv(pov->m_sockClient);
		break;
	case IO_RECV:
		//投递一个接收数据的请求
		printf("接收到数据%s\r\n", pov->m_btBuf);
		PostRecv(pov->m_sockClient);
		break;
	default:
		break;
	}

}
142 }
}

void Get_Hallinfo(HWND& room_list, HWND& user_list) {
	//if (!isconnecting())
	//	return;
	(int)SendMessage(user_list, LB_RESETCONTENT, 0, 0);
	{
		string s = "Getuser";
		string re = send_socket(s);
		if (re == "error") return;
		regex user_reg("[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+");
		string temp;
		wstring wtemp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
			wtemp = string2wstring((*iter)[0]);
			(int)SendMessage(user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));
		}
	}
	{
		string s = "Getroom";
	}
}

void Send_Message(wstring& ws) {
	string s = wstring2string(ws);
	s = "send:" + s;
	send(mysocket, &(s[0]), 1023, 0);
}