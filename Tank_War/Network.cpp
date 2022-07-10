#include"Network.h"
#include"Game.h"

extern STATUS status;

extern wstring my_userid;

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


void send_socket(string s) {
	char* send_buf = &(s[0]);
	int i = send(mysocket, send_buf, 1023, 0);
}

void set_tankid(string& s)
{
	string::const_iterator iterStart = s.begin();
	string::const_iterator iterEnd = s.end();
	smatch m;
	regex reg("[0-9]+");
	regex_search(iterStart, iterEnd, m, reg);
	string temp;
	temp = m[0];
	int id = atoi(temp.c_str());
	set_My_id(id);
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
				SendMessage(_hwnd, WM_COMMAND, DISBANDINROOM, (LPARAM)_hwnd);
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
			else if (temp == "tankid")
			{
				string recv_str(m[0].second + 1, iterEnd);
				set_tankid(recv_str);
			}
		}
	}
	else
	{
		if (temp == "tankinfo")
		{
			Cur_Game->refrash_tankinfo(buf);
		}
		else if (temp == "bulletinfo")
		{
			Cur_Game->refrash_bullet(buf);
		}
		else if (temp == "destroyed")
		{
			Cur_Game->destoryed(buf);
		}
		else if (temp == "youdestroyed")
		{
			Cur_Game->mydestoryed();
		}
		else if (temp == "wingame")
		{
			SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
		}
		else if (temp == "failgame")
		{
			SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
		}
		else if (temp == "disband")
		{
			SendMessage(_hwnd, WM_COMMAND, DISBANDINEND, (LPARAM)_hwnd);
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