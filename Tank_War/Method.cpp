#include"Method.h"

extern STATUS status;

extern wstring my_userid;


//extern HWND hwndButton1;
//extern HWND hwndButton2;
//extern HWND hwndButton3;
//extern HWND hwndButton4;
//extern HWND hwndButton5;
//extern HWND hwndButton6;


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

//extern void Refresh_opTank(char buf[]);
//extern void Refresh_opbullet(string& re);
//extern void my_destroy();
//extern void op_destory();

queue<Ping_info> ping_queue;
int delay = 0;
int ping_id = 0;

queue<socket_messageinfo*> message_queue;
mutex messagequeue_mutex;
bool Process_Stop = false;

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

//void Show_Main_UI() {
//	ShowWindow(hwndButton2, SW_SHOW);
//	ShowWindow(hwndButton3, SW_SHOW);
//	ShowWindow(hwndButton4, SW_SHOW);
//	ShowWindow(hwndButton5, SW_SHOW);
//}

//void Hide_Main_UI() {
//	ShowWindow(hwndButton2, SW_HIDE);
//	ShowWindow(hwndButton3, SW_HIDE);
//	ShowWindow(hwndButton4, SW_HIDE);
//	ShowWindow(hwndButton5, SW_HIDE);
//	//ShowWindow(Hall, SW_HIDE);
//	ShowWindow(Room, SW_HIDE);
//}

//void SIG_IO(int sig) {
//
//}

//bool isconnecting() {
//	int i = recv(mysocket, buffer, 1023, MSG_PEEK);
//	if (i == 0)return false;
//	else if (i == -1) {
//		return (errno == EINTR);
//	}
//	return true;
//}

void Return_To_Mune() {
	isstart = false;
}

void send_socket(string s) {
	char* send_buf = &(s[0]);
	int i = send(mysocket, send_buf, 1023, 0);
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
		if (status == STATUS::Hall_Status)
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
		else if (status == STATUS::Room_Status)
		{
			if (temp == "Start")
			{
				SendMessage(_hwnd, WM_COMMAND, START, (LPARAM)_hwnd);
			}
			else if (temp == "disband")
			{
				SendMessage(_hwnd, WM_COMMAND, QUITROOM, (LPARAM)_hwnd);
			}
			else if (temp == "Roomuser")
			{
				string recv_str(m[0].second + 1, iterEnd);
				Return_Get_Room_User(recv_str);
			}
			else if (temp == "RoomMessage")
			{
				string recv_str(m[0].second + 1, iterEnd);
				Return_Room_Message(recv_str);
			}
		}
	}
	else
	{
		if (temp == "oplocation")
		{
			//Refresh_opTank(buf);
		}
		else if (temp == "opbullet")
		{
			string recv_str(m[0].second + 1, iterEnd);
			//Refresh_opbullet(recv_str);
		}
		else if (temp == "destroy")
		{
			//my_destroy();
			lost_game();
		}
		else if (temp == "opdestroy")
		{
			//op_destory();
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
	static clock_t last_refrash = 0;
	static clock_t now_refrash = 0;
	now_refrash = clock();
	if (now_refrash - last_refrash <= 1000)
		return;
	last_refrash = now_refrash;
	send_socket("Getuser");
	send_socket("Getroom");
}

void Return_Get_Hallinfo_User(string& re) {
	(int)SendMessage(Hall_user_list, LB_RESETCONTENT, 0, 0);
	wstring wtemp = my_userid + L"(您)";
	(int)SendMessage(Hall_user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));
	{
		regex user_reg("(#)[^;]*");
		string temp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
			wtemp = string2wstring((*iter)[0]);
			(int)SendMessage(Hall_user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[1]));
		}
	}
}

void Return_Get_Hallinfo_Room(string re) {
	(int)SendMessage(Hall_room_list, LB_RESETCONTENT, 0, 0);
	{
		regex room_reg("(#)[^;]*");
		string temp;
		wstring wtemp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), room_reg); iter != end; iter++)
		{
			string s = (*iter)[0];
			s += "的房间";
			wtemp = string2wstring(s);
			(int)SendMessage(Hall_room_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[1]));
		}
	}
}

void Return_Get_Hallinfo_Roomid(string re) {
	regex roomid_reg("[0-9]+");
	sregex_iterator end;
	int counter = 0;
	for (sregex_iterator iter(re.begin(), re.end(), roomid_reg); iter != end; iter++) {
		string str = (*iter)[0];
		int id = atoi(str.c_str());
		room_id[counter] = id;
		counter++;
	}
	room_count = counter + 1;
}

void Send_Hall_Message(wstring& w_content) {
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
	SendMessage(Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	//发送消息到文本框
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

	//发送消息
	string s = wstring2string(w_content);
	s = "HallSend:" + s;
	send(mysocket, &(s[0]), 1023, 0);
}

mutex process_mtx;
condition_variable process_cv;
DWORD WINAPI Process_Thread() {
	while (!Process_Stop)
	{
		unique_lock<mutex> lck(process_mtx);
		process_cv.wait(lck);
		while (!message_queue.empty())
		{
			unique_lock<mutex> qlck(messagequeue_mutex);
			socket_messageinfo* pinfo = message_queue.front();
			message_queue.pop();
			qlck.unlock();
			Return_Class(pinfo->ch);
		}
		lck.unlock();
	}
	return 0;
}

DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam) {
	thread T2(Process_Thread);
	HANDLE hIOCP = (HANDLE)lpParam;
	DWORD dwBytesTranfered = 0;
	long long pPerHandle;
	while (true) {
		bool bl = ::GetQueuedCompletionStatus(hIOCP, &dwBytesTranfered, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED*)&pPerIO, WSA_INFINITE);
		if (!bl)
		{
			closesocket(mysocket);
			GlobalFree(pPerIO);
			break;
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
			socket_messageinfo* pinfo = new socket_messageinfo(pPerIO->buf);
			unique_lock<mutex> qlck(messagequeue_mutex);
			message_queue.push(pinfo);
			qlck.unlock();
			process_cv.notify_one();
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
	Process_Stop = true;
	process_cv.notify_one();
	T2.join();
	return 0;
}

void Create_Room() {
	send_socket("CreateRoom");
	host = true;
	Set_CurScene(STATUS::Room_Status);
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

HBRUSH OnCtlColorEdit(WPARAM wParam, LPARAM lParam)
{
	HWND hEdit1, hedit2;
	hEdit1 = ::GetDlgItem(_hwnd, 1017);
	HDC hDc = GetDC(Hall_room_list);

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

bool Init_Hall()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	ZeroMemory(&addr_info, sizeof(addr_info));
	addr_info.sin_family = AF_INET;
	addr_info.sin_port = htons(DEFAULT_PORT);
	inet_pton(AF_INET, SERVER_IP, &(addr_info.sin_addr.S_un.S_addr));

	memset(buffer, '\0', 1024);
	mysocket = WSASocket(addr_info.sin_family, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	int RecvBuf = 1024 * 1024;
	int SendBuf = 1024 * 1024;

	//setsockopt(mysocket, SOL_SOCKET, SO_SNDBUF, (const char*)&SendBuf, sizeof(int));
	//setsockopt(mysocket, SOL_SOCKET, SO_RCVBUF, (const char*)&RecvBuf, sizeof(int));
	int flag = 1;
	setsockopt(mysocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));

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
	Process_Stop = false;
	thread T1(Recv_Thread, Hall_pPerIO, LPVOID(Hall_hIOCP));
	T1.detach();
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
				//delay = recv_id;
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
	//delay = ping_id;

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

void ReturnToRoom()
{
	send_socket("returntoroom");
}

void setmyuserid()
{
	string str = "myuserid:" + wstring2string(my_userid);
	send_socket(str);
}

void Get_Room_Info()
{
	send_socket("GetRoominfo");
}

void Return_Get_Room_User(string& re)
{
	(int)SendMessage(Room_user_list, LB_RESETCONTENT, 0, 0);
	wstring wtemp = my_userid + L"(您)";
	if (host)
	{
		wtemp += L"（房主）";
	}
	else if (isready)
	{
		wtemp += L"（已准备）";
	}
	(int)SendMessage(Room_user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));

	{
		regex user_reg("(#)[^;]*");
		string temp;
		sregex_iterator end;
		for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++)
		{
			string s = (*iter)[0];

			string::const_iterator iterStart = s.begin();
			string::const_iterator iterEnd = s.end();
			regex inter_reg("[:]{2}");
			smatch m;
			regex_search(iterStart, iterEnd, m, inter_reg);
			string ready_str(m[0].second, m[0].second + 1);
			string name_str(iterStart + 1, m[0].first);

			if (ready_str == "1")
			{
				name_str += "（已准备）";
			}
			else if (ready_str == "2")
			{
				name_str += "（房主）";
			}
			wstring name = string2wstring(name_str);
			(int)SendMessage(Room_user_list, LB_ADDSTRING, 0, (LPARAM) & (name[0]));
		}
	}
}

void Room_Ready()
{
	send(mysocket, "Ready", 1023, 0);
	isready = true;
	SRoom_nothost->ModifyButton_ID(IDB_READY, IDB_CANCELREADY);
	SRoom_nothost->ModifyText_byButton(IDB_CANCELREADY, L"取消准备");
}
void Room_CancelReady()
{
	send(mysocket, "CancelReady", 1023, 0);
	isready = false;
	SRoom_nothost->ModifyButton_ID(IDB_CANCELREADY, IDB_READY);
	SRoom_nothost->ModifyText_byButton(IDB_READY, L"准备");
}

void Set_CurScene(STATUS status_in)
{
	Show_Hall(false);
	Show_Room(false);
	switch (status_in)
	{
	case STATUS::Room_Status:
	{
		isonline_game = false;
		isstart = false;
		isready = false;
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

void Send_Room_Message(wstring& w_content) {
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
	SendMessage(Room_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	//发送消息到文本框
	SendMessage(edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(edit_room, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

	//发送消息
	string s = wstring2string(w_content);
	s = "RoomSend:" + s;
	send(mysocket, &(s[0]), 1023, 0);
}

void Return_Room_Message(string& recv_str) {
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
	SendMessage(edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(edit_room, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}