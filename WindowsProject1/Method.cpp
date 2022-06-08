#include"Method.h"

extern Tank* mytank;
extern Tank* optank;

extern bool isstart;
extern HWND hwndButton1;
extern HWND hwndButton2;
extern HWND hwndButton3;
extern HWND hwndButton4;
extern HWND hwndButton5;
extern HWND hwndButton6;
extern HWND room_list;
extern HWND user_list;
extern HWND edit_hall;
extern HWND edit_in;

extern HWND _hwnd;
extern HDC hdc;

extern HWND Hall;
extern HWND Room;

extern SOCKET mysocket;
extern char buffer[1024];

int room_id[65535];
int room_count = 0;

extern bool Hall_IOCP_flag;
extern bool Game_IOCP_flag;

//void START(HWND hWnd) {}

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
	ShowWindow(Hall, SW_HIDE);
	ShowWindow(Room, SW_HIDE);
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

void send_socket(string s) {
	const char* send_buf = &(s[0]);
	send(mysocket, send_buf, 1023, 0);
}

void Return_Class(char buf[]) {
	string re = buf;
	string::const_iterator iterStart = re.begin();
	string::const_iterator iterEnd = re.end();
	smatch m;
	regex reg("^[A-Z|a-z]+");
	regex_search(iterStart, iterEnd, m, reg);
	string temp;
	temp = m[0];
	if (!isstart)
	{
		if (temp == "user")
		{
			string recv_str(m[0].second + 1, iterEnd);
			Return_Get_Hallinfo_User(recv_str);
		}
		else if (temp == "HallMessage")
		{
			string recv_str(m[0].second + 1, iterEnd);
			Return_Hallinfo_Message(recv_str);
		}
		else if (temp == "room")
		{
			string recv_str(m[0].second + 1, iterEnd);
			Return_Get_Hallinfo_Room(recv_str);
		}
		else if (temp == "roomid")
		{
			string recv_str(m[0].second + 1, iterEnd);
			Return_Get_Hallinfo_Roomid(recv_str);
		}
		else if (temp == "EnterRoom")
		{
			SendMessage(Hall, WM_COMMAND, Enterroom, (LPARAM)_hwnd);
		}
		else if (temp == "Start")
		{
			SendMessage(Room, WM_COMMAND, START, (LPARAM)_hwnd);
		}
	}
	else
	{
		if (temp == "oplocation")
		{
			try
			{
				memcpy(optank, &buf[11], 24);
			}
			catch (exception& e)
			{
				return;
			}
		}
		else if (temp == "opbullet")
		{
			string recv_str(m[0].second + 1, iterEnd);
			Refresh_opbullet(recv_str);
		}
	}
}

void Return_Hallinfo_Message(string& recv_str) {
	string::const_iterator iterStart = recv_str.begin();
	string::const_iterator iterEnd = recv_str.end();
	smatch _content;
	regex message_reg("_content");
	regex_search(iterStart, iterEnd, _content, message_reg);
	string user_id(iterStart, _content[0].first - 1);
	user_id += ";\r\n";
	string content(_content[0].second + 1, iterEnd);
	wstring w_user_id = string2wstring(user_id);
	wstring w_content = string2wstring(content);
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_id[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

void Get_Hallinfo() {
	send_socket("Getuser");
	send_socket("Getroom");
}

void Return_Get_Hallinfo_User(string& re) {
	(int)PostMessage(user_list, LB_RESETCONTENT, 0, 0);
	{
		regex user_reg("[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+");
		string temp;
		wstring wtemp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
			wtemp = string2wstring((*iter)[0]);
			(int)PostMessage(user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));
		}
	}
}

void Return_Get_Hallinfo_Room(string re) {
	(int)SendMessage(room_list, LB_RESETCONTENT, 0, 0);
	{
		regex user_reg("[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+");
		string temp;
		wstring wtemp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
			string s = (*iter)[0];
			s += "的房间";
			wtemp = string2wstring(s);
			(int)SendMessage(room_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));
		}
	}
}

void Return_Get_Hallinfo_Roomid(string re) {
	regex user_reg("[0-9]+");
	sregex_iterator end;
	int counter = 0;
	for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
		string str = (*iter)[0];
		int id = atoi(str.c_str());
		room_id[counter] = id;
		counter++;
	}
	room_count = counter + 1;
}

void Send_Message(wstring& ws) {
	string s = wstring2string(ws);
	SendMessage(edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"我:\r\n");
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (ws[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
	s = "HallSend:" + s;
	send(mysocket, &(s[0]), 1023, 0);
}

DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam) {
	HANDLE hIOCP = (HANDLE)lpParam;
	DWORD dwBytesTranfered = 0;
	int pPerHandle;
	while (true) {
		bool bl = ::GetQueuedCompletionStatus(hIOCP, &dwBytesTranfered, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED*)&pPerIO, WSA_INFINITE);
		if (!bl)
		{
			closesocket(mysocket);
			GlobalFree(pPerIO);
			continue;
		}
		if (dwBytesTranfered == 0 && (pPerIO->nOperationType == OP_READ || pPerIO->nOperationType == OP_WRITE)) {
			closesocket(mysocket);
			GlobalFree(pPerIO);
			continue;
		}
		switch (pPerIO->nOperationType)
		{   //通过per-IO数据中的nOperationType域查看有什么I/O请求完成了
		case OP_READ:  //完成一个接收请求
		{
			pPerIO->buf[dwBytesTranfered] = '\0';
			Return_Class(pPerIO->buf);
			WSABUF buf;
			buf.buf = pPerIO->buf;
			buf.len = 1024;
			pPerIO->nOperationType = OP_READ;
			DWORD nFlags = 0;
			::WSARecv(mysocket, &buf, 1, &dwBytesTranfered, &nFlags, &pPerIO->ol, NULL);
		}
		break;
		case OP_WRITE:break;
		}
	}
	return 0;
}

void Create_Room() {
	send_socket("CreateRoom");
}

void Enter_Room(int index) {
	string s = "EnterRoom:" + to_string(room_id[index]);
	send_socket(s);
}

void send_location(Tank* tank)
{
	char buffer[1024] = "mylocation:";
	int i = sizeof(Tank);
	memcpy(&buffer[11], (char*)tank, sizeof(Tank));
	send(mysocket, buffer, 1023, 0);
}

void send_bullet(bullet* cur)
{
	string str = "mybullet:";
	while (cur != NULL)
	{
		str = str + "{"
			+ to_string(cur->locationX)
			+ ","
			+ to_string(cur->locationY)
			+ "}";
		cur = cur->next;
	}
	send_socket(str);
}

void Refresh_opTank(char buf[])
{
	try
	{
		memcpy(optank, &buf[11], 24);
	}
	catch (exception& e)
	{
		return;
	}
}

void Refresh_opbullet(string& re)
{
	bullet* newhead = new bullet();
	bullet* temp = newhead;

	regex user_reg("[0-9]+");
	sregex_iterator end;
	for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
		string s1 = ((*iter)[0]);
		if (iter != end)
			iter++;
		else break;
		string s2 = ((*iter)[0]);
		if (temp->next == NULL)
		{
			temp->next = new bullet();
			temp = temp->next;
			temp->locationX = atoi(s1.c_str());
			temp->locationY = atoi(s2.c_str());
			temp->speed = 20;
			temp->owner = optank;
		}
	}
	optank->bullet_head = newhead->next;
}