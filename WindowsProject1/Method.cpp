#include"Method.h"

extern bool isstart;

extern int status;

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

extern WSADATA wsa;
extern sockaddr_in addr_info;
extern SOCKET mysocket;

extern HANDLE Hall_hIOCP;
extern PPER_IO_DATA Hall_pPerIO;

extern void Refresh_opTank(char buf[]);
extern void Refresh_opbullet(string& re);
extern void my_destroy();
extern void op_destory();

queue<Ping_info> ping_queue;
int delay = 0;
int ping_id = 0;

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

//void Get_Init_UI(HWND hWnd) {
//	RECT rect;
//	GetClientRect(hWnd, &rect);
//	{
//		hwndButton1 = CreateWindow(
//			L"BUTTON",	// predefined class    不区分大小写
//			L"暂停",       // button text
//			WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示
//
//		   // Size and position values are given explicitly, because
//		   // the CW_USEDEFAULT constant gives zero values for buttons.
//			10,         // starting x position
//			10,         // starting y position
//			80,        // button width
//			30,        // button height
//			hWnd,       // parent window
//			(HMENU)IDB_ONE,       // No menu
//			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
//			NULL);      // pointer not needed
//		ShowWindow(hwndButton1, SW_HIDE);
//	}
//	{
//		hwndButton2 = CreateWindow(
//			L"BUTTON",	// predefined class    不区分大小写
//			L"本地游戏",       // button text
//			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示
//
//		   // Size and position values are given explicitly, because
//		   // the CW_USEDEFAULT constant gives zero values for buttons.
//			(rect.left + rect.right) / 2 - 90,         // starting x position
//			80,         // starting y position
//			180,        // button width
//			50,        // button height
//			hWnd,       // parent window
//			(HMENU)IDB_TWO,       // No menu
//			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
//			NULL);      // pointer not needed
//	}
//	{
//		hwndButton3 = CreateWindow(
//			L"BUTTON",	// predefined class    不区分大小写
//			L"联机大厅",       // button text
//			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示
//
//		   // Size and position values are given explicitly, because
//		   // the CW_USEDEFAULT constant gives zero values for buttons.
//			(rect.left + rect.right) / 2 - 90,         // starting x position
//			180,         // starting y position
//			180,        // button width
//			50,        // button height
//			hWnd,       // parent window
//			(HMENU)IDB_THREE,       // No menu
//			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
//			NULL);      // pointer not needed
//	}
//	{
//		hwndButton4 = CreateWindow(
//			L"BUTTON",	// predefined class    不区分大小写
//			L"设置",       // button text
//			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示
//
//		   // Size and position values are given explicitly, because
//		   // the CW_USEDEFAULT constant gives zero values for buttons.
//			(rect.left + rect.right) / 2 - 90,         // starting x position
//			280,         // starting y position
//			180,        // button width
//			50,        // button height
//			hWnd,       // parent window
//			(HMENU)IDB_FOUR,       // No menu
//			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
//			NULL);      // pointer not needed
//	}
//	{
//		hwndButton5 = CreateWindow(
//			L"BUTTON",	// predefined class    不区分大小写
//			L"退出游戏",       // button text
//			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles     注意 如果样式写错了 Button 将不会正常显示
//
//		   // Size and position values are given explicitly, because
//		   // the CW_USEDEFAULT constant gives zero values for buttons.
//			(rect.left + rect.right) / 2 - 90,         // starting x position
//			380,         // starting y position
//			180,        // button width
//			50,        // button height
//			hWnd,       // parent window
//			(HMENU)IDB_FIVE,       // No menu
//			(HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE),
//			NULL);      // pointer not needed
//	}
//
//}

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
	//ShowWindow(Hall, SW_HIDE);
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
	if (temp == "ping")
	{
		string recv_str(m[0].second + 1, iterEnd);
		Ping_Count(recv_str);
		return;
	}
	if (!isstart)
	{
		if (status == Hall_Status)
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
				SendMessage(_hwnd, WM_COMMAND, Enterroom, (LPARAM)_hwnd);
			}
		}
		else if (status == Room_Status)
		{
			if (temp == "Start")
			{
				SendMessage(Room, WM_COMMAND, START, (LPARAM)_hwnd);
			}
			if (temp == "QuitRoom")
			{
				SendMessage(Room, WM_COMMAND, QUITROOM, (LPARAM)_hwnd);
			}
		}
	}
	else
	{
		if (temp == "oplocation")
		{
			Refresh_opTank(buf);
		}
		else if (temp == "opbullet")
		{
			string recv_str(m[0].second + 1, iterEnd);
			Refresh_opbullet(recv_str);
		}
		else if (temp == "destroy")
		{
			my_destroy();
			lost_game();
		}
		else if (temp == "opdestroy")
		{
			op_destory();
			win_game();
		}
	}
}

void Return_Hallinfo_Message(string& recv_str) {
	string::const_iterator iterStart = recv_str.begin();
	string::const_iterator iterEnd = recv_str.end();
	smatch _content;
	regex message_reg("_content");
	regex_search(iterStart, iterEnd, _content, message_reg);

	//获取当前时间并格式化为字符串
	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	struct tm t;   //tm结构指针
	auto time_tm = localtime_s(&t, &tt);
	char charTime[25] = { 0 };
	sprintf_s(charTime, "%d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900,
		t.tm_mon + 1, t.tm_mday, t.tm_hour,
		t.tm_min, t.tm_sec);

	//消息头(用户ID 时间;)
	string strTime = charTime;
	string user_head(iterStart, _content[0].first - 1);
	user_head += "  ";
	user_head += strTime;
	user_head += ";\r\n";
	wstring w_user_head = string2wstring(user_head);

	//消息内容
	string content(_content[0].second + 1, iterEnd);
	wstring w_content = string2wstring(content);

	//发送消息到文本框
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

void Get_Hallinfo() {
	send_socket("Getuser");
	send_socket("Getroom");
}

void Return_Get_Hallinfo_User(string& re) {
	(int)SendMessage(user_list, LB_RESETCONTENT, 0, 0);
	wstring wtemp = L"unname(您)";
	(int)SendMessage(user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));
	{
		regex user_reg("[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+");
		string temp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
			wtemp = string2wstring((*iter)[0]);
			(int)SendMessage(user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));
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
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++)
		{
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

void Send_Message(wstring& w_content) {
	//获取当前时间并格式化为字符串
	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	struct tm t;   //tm结构指针
	auto time_tm = localtime_s(&t, &tt);
	char charTime[25] = { 0 };
	sprintf_s(charTime, "%d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900,
		t.tm_mon + 1, t.tm_mday, t.tm_hour,
		t.tm_min, t.tm_sec);
	string strTime = charTime;

	string user_head = "我  ";
	user_head += strTime;
	user_head += ":\r\n";

	wstring w_user_head = string2wstring(user_head);
	//清空输入文本框
	SendMessage(edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	//发送消息到文本框
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

	//发送消息
	string s = wstring2string(w_content);
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
			//pPerIO->buf[dwBytesTranfered] = '\0';
			Return_Class(pPerIO->buf);
			WSABUF buf;
			buf.buf = pPerIO->buf;
			buf.len = 1023;
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

void win_game()
{
	SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
}

void lost_game()
{
	SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
}

HRESULT Loadbitmap(IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget, LPCTSTR pszResource, ID2D1Bitmap** ppBitmap)
{
	if (NULL == pIWICFactory)
	{
		CoInitialize(NULL);
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pIWICFactory)
		);
	}
	HRESULT hr = S_OK;
	IWICStream* pStream = NULL;
	IWICBitmapScaler* pScaler = NULL;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICFormatConverter* pConverter = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(
		pszResource,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}

	SafeRelease(pDecoder);
	SafeRelease(pSource);
	SafeRelease(pStream);
	SafeRelease(pConverter);
	SafeRelease(pScaler);

	return hr;
}

HBRUSH OnCtlColorEdit(WPARAM wParam, LPARAM lParam)
{
	HWND hEdit1, hedit2;
	hEdit1 = ::GetDlgItem(_hwnd, 1017);
	HDC hDc = GetDC(room_list);

	if (hEdit1 == (HWND)lParam)
	{
		::SetTextColor(hDc, RGB(0, 0, 255)); //RGB(0, 0, 255)
		::SetBkMode(hDc, TRANSPARENT);
		//SetBkColor(RGB(125, 255, 0)); //文字背景
		HBRUSH hbr = (HBRUSH)::GetStockObject(NULL_BRUSH); //编辑框背景。注意：和文字背景不是一个意思。
		//TRANSPARENT,OPAQUE

		//说明:TRANSPARENT是设置背景透明,但是控件多行滚动时有重影,

		//还没弄明白,所以用的OPAOUE,这也不明白是啥玩意,但能达到目的就OK,

		//::SetBkColor(hDc,RGB(255,0,0));
		return hbr;//返回背景色的画刷
	}
	ReleaseDC(hEdit1, hDc);
	return 0;
}

void Show_Hall(bool flag)
{
	if (flag)
	{
		ShowWindow(edit_hall, SW_SHOW);
		ShowWindow(edit_in, SW_SHOW);
		ShowWindow(room_list, SW_SHOW);
		ShowWindow(user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(edit_hall, SW_HIDE);
		ShowWindow(edit_in, SW_HIDE);
		ShowWindow(room_list, SW_HIDE);
		ShowWindow(user_list, SW_HIDE);
	}
}

bool Init_Hall()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	ZeroMemory(&addr_info, sizeof(addr_info));
	addr_info.sin_family = AF_INET;
	addr_info.sin_port = htons(DEFAULT_PORT);
	inet_pton(AF_INET, SERVER_IP, &(addr_info.sin_addr.S_un.S_addr));

	memset(buffer, '\0', 1024);
	mysocket = WSASocket(addr_info.sin_family, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (-1 == connect(mysocket, (struct sockaddr*)&addr_info, sizeof(struct sockaddr)))
	{
		MessageBoxW(_hwnd, L"无法连接服务器，请检查网络设置", L"网络连接错误", NULL);
		return false;
	}
	unsigned long ul = 1;
	ioctlsocket(mysocket, FIONBIO, &ul);

	Hall_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort((HANDLE)mysocket, Hall_hIOCP, NULL, 0);
	Hall_pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
	Hall_pPerIO->nOperationType = OP_READ;
	WSABUF buf;
	buf.buf = Hall_pPerIO->buf;
	buf.len = 1023;
	DWORD dwRecv;
	DWORD dwFlags = 0;
	WSARecv(mysocket, &buf, 1, &dwRecv, &dwFlags, &Hall_pPerIO->ol, NULL);
	thread T(Recv_Thread, Hall_pPerIO, LPVOID(Hall_hIOCP));
	T.detach();
	return true;
}

void Ping_Count(string& str)
{
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch()
		);
	int recv_time = ms.count();
	try
	{
		int recv_id = atoi(str.c_str());
		while (!ping_queue.empty())
		{
			if (ping_queue.front().id > recv_id)
			{
				return;
			}
			else if (ping_queue.front().id == recv_id)
			{
				delay = min((recv_time - ping_queue.front().send_time) / 2, 999);
				ping_queue.pop();
				return;
			}
			else
			{
				ping_queue.pop();
			}
		}
	}
	catch (const std::exception&)
	{
		return;
	}
}

void send_pingmessage()
{
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch()
		);
	int time = ms.count();

	Ping_info ping_info;
	ping_info.id = ping_id;
	ping_info.send_time = time;
	ping_queue.push(ping_info);

	send_socket("ping:" + to_string(ping_id));
	if (ping_id != INT32_MAX)
	{
		ping_id++;
	}
	else
	{
		ping_id = 0;
		while (!ping_queue.empty())
		{
			ping_queue.pop();
		}
	}
}