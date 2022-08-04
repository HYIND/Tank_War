#include"Network.h"
#include"Game.h"

extern STATUS status;

wstring my_userid = L"unname";

extern HWND _hwnd;
extern HDC hdc;

extern HWND Hall;
extern HWND Room;

int room_id[65535];
//int room_count = 0;

WSADATA wsa;
sockaddr_in addr_info;
SOCKET mysocket = INVALID_SOCKET;
char buffer[1024];


HANDLE hIOCP;
PPER_IO_DATA pPerIO;

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

int Get_Header_Type_bystring(string& str)
{
	if (str == "GetHallinfo")return 101;
	else if (str == "CreateRoom") return 104;
	else if (str == "GetRoominfo") return 110;
	else if (str == "Ready") return 111;
	else if (str == "CancelReady") return 112;
	else if (str == "StartGame")return 115;
	else if (str == "QuitRoom") return 116;
}

void send_string(string s) {
	Header header;
	header.type = Get_Header_Type_bystring(s);
	if (header.type == 0)
		return;
	header.length = 0;
	char send_buf[sizeof(Header)] = { '\0' };
	memcpy(send_buf, &header, sizeof(Header));
	send(mysocket, send_buf, sizeof(Header), 0);
}

//void Return_Class(char buf[]) {
//	string re = buf;
//	string::const_iterator iterStart = re.begin();
//	string::const_iterator iterEnd = re.end();
//	smatch m;
//	regex reg("^[A-Z|a-z]+");
//	regex_search(iterStart, iterEnd, m, reg);
//	string temp;
//	temp = m[0];
//	if (temp == "ping")
//	{
//		string recv_str(m[0].second + 1, iterEnd);
//		Ping_Count(recv_str);
//		return;
//	}
//	if (!isstart)
//	{
//		if (status == STATUS::Hall_Status)
//		{
//			if (temp == "user")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
//				Return_Get_Hallinfo_User(recv_str);
//			}
//			else if (temp == "HallMessage")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
//				Return_Hallinfo_Message(recv_str);
//			}
//			else if (temp == "room")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
				//Return_Get_Hallinfo_Room(recv_str);
//			}
//			else if (temp == "roomid")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
//				Return_Get_Hallinfo_Roomid(recv_str);
//			}
//			else if (temp == "EnterRoom")
//			{
//				SendMessage(_hwnd, WM_COMMAND, Enterroom, (LPARAM)_hwnd);
//			}
//		}
//		else if (status == STATUS::Room_Status)
//		{
//			if (temp == "Start")
//			{
//				SendMessage(_hwnd, WM_COMMAND, START, (LPARAM)_hwnd);
//			}
//			else if (temp == "disband")
//			{
//				SendMessage(_hwnd, WM_COMMAND, DISBANDINROOM, (LPARAM)_hwnd);
//			}
//			else if (temp == "Roomuser")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
//				Return_Get_Room_User(recv_str);
//			}
//			else if (temp == "RoomMessage")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
//				Return_Room_Message(recv_str);
//			}
//			else if (temp == "tankid")
//			{
//				string recv_str(m[0].second + 1, iterEnd);
//				set_tankid(recv_str);
//			}
//		}
//	}
//	else if (status == STATUS::Game_Status)
//	{
//		if (temp == "tankinfo")
//		{
//			Cur_Game->refreash_tankinfo(buf);
//		}
//		else if (temp == "bulletinfo")
//		{
//			Cur_Game->refreash_bullet(buf);
//		}
//		else if (temp == "hitbrick")
//		{
//			Cur_Game->recv_hitbrick(buf);
//		}
//		else if (temp == "hited")
//		{
//			Cur_Game->recv_hited(buf);
//		}
//		else if (temp == "youhited")
//		{
//			Cur_Game->recv_myhited();
//		}
//		else if (temp == "destroyed")
//		{
//			Cur_Game->recv_destoryed(buf);
//		}
//		else if (temp == "youdestroyed")
//		{
//			Cur_Game->recv_mydestoryed();
//		}
//		else if (temp == "wingame")
//		{
//			SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
//		}
//		else if (temp == "failgame")
//		{
//			SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
//		}
//		else if (temp == "disband")
//		{
//			SendMessage(_hwnd, WM_COMMAND, DISBANDINEND, (LPARAM)_hwnd);
//		}
//	}
//}

void Return_Class(socket_messageinfo* info)
{
	if (info->header.type == 206)
	{
		Ping_Count(info->header, info->content);
		return;
	}
	switch (status)
	{
	case STATUS::Hall_Status:
	{
		switch (info->header.type)
		{
		case 201:
			Return_Get_Hallinfo(info->header, info->content);
			break;
		case 203:
			Return_Hall_Message(info->header, info->content);
			break;
		case 205:
			Return_Enter_Room(info->header, info->content);
			break;
		}
		break;
	}
	case STATUS::Room_Status:
	{
		switch (info->header.type)
		{
		case 210:
			Return_Get_Roominfo(info->header, info->content);
			break;
		case 213:
			Return_Room_Message(info->header, info->content);
			break;
		case 214:
			Return_Set_tankid(info->header, info->content);
			break;
		case 215:
			Return_Start(info->header, info->content);
			break;
		case 216:
			SendMessage(_hwnd, WM_COMMAND, DISBANDINROOM, (LPARAM)_hwnd);
			break;
		}
		break;
	}
	case STATUS::Game_Status:
	{

		switch (info->header.type)
		{
		case 220:
			Cur_Game->refreash_tankinfo(info->header, info->content);
			break;
		case 221:
			Cur_Game->refreash_bullet(info->header, info->content);
			break;
		case 222:
			Cur_Game->recv_hitbrick(info->header, info->content);
			break;
		case 223:
			Cur_Game->recv_hited(info->header, info->content);
			break;
		case 224:
			Cur_Game->recv_myhited();
			break;
		case 225:
			Cur_Game->recv_destoryed(info->header, info->content);
			break;
		case 226:
			Cur_Game->recv_mydestoryed();
			break;
		case 227:
			SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
			break;
		case 228:
			SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
			break;
		case 229:
			SendMessage(_hwnd, WM_COMMAND, DISBANDINEND, (LPARAM)_hwnd);
			break;
		}
	}
	break;
	}
}


void Return_Get_Hallinfo(Header& header, char* content) {
	Message::Hall_info_Response Res;
	Res.ParseFromArray(content, header.length);

	{
		(int)SendMessage(SHall->Hall_user_list, LB_RESETCONTENT, 0, 0);
		wstring w_name = my_userid + L"(您)";
		(int)SendMessage(SHall->Hall_user_list, LB_ADDSTRING, 0, (LPARAM) & (w_name[0]));
		string name;
		for (int i = 0; i < Res.userinfo_size(); i++) {
			Message::Hall_info_Response_User userinfo = Res.userinfo(i);
			name = userinfo.name();
			w_name = utf82wstring(name);
			(int)SendMessage(SHall->Hall_user_list, LB_ADDSTRING, 0, (LPARAM) & (w_name[0]));
		}
	}

	{
		(int)SendMessage(SHall->Hall_room_list, LB_RESETCONTENT, 0, 0);

		string name;
		wstring w_name;
		int counter = 0;
		for (int i = 0; i < Res.roominfo_size(); i++) {
			Message::Hall_info_Response_Roominfo roominfo = Res.roominfo(i);
			int id = roominfo.room_id();
			name = roominfo.host_name();
			w_name = utf82wstring(name) + L"的房间";
			room_id[counter] = id;
			counter++;
			(int)SendMessage(SHall->Hall_room_list, LB_ADDSTRING, 0, (LPARAM) & (w_name[0]));
		}
	}
}

void Return_Hall_Message(Header& header, char* content) {
	Message::Hall_Message_Response Res;
	Res.ParseFromArray(content, header.length);

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
	string message_head = Res.name() + " " + strTime + ":\r\n";
	wstring w_message_head = utf82wstring(message_head);

	//消息内容
	string message_content = Res.content();
	wstring w_message_content = utf82wstring(message_content);

	//发送消息到文本框
	SendMessage(SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_message_head[0]));
	SendMessage(SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_message_content[0]));
	SendMessage(SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

void Return_Enter_Room(Header& header, char* content) {
	Message::Hall_EnterRoom_Response Res;
	Res.ParseFromArray(content, header.length);
	switch (Res.result())
	{
	case 1:
	{
		SendMessage(_hwnd, WM_COMMAND, Enterroom, (LPARAM)_hwnd);
		break;
	}
	case 0:
	{
		MessageBox(_hwnd, L"房间人数已满", NULL, MB_OK);
		break;
	}
	case -1:
	{
		MessageBox(_hwnd, L"房间不存在/已解散", NULL, MB_OK);
	}
	}
}

void Get_Hallinfo() {
	static clock_t last_refreash = 0;
	static clock_t now_refreash = 0;
	now_refreash = clock();
	if (now_refreash - last_refreash <= 1000)
		return;
	last_refreash = now_refreash;
	send_string("GetHallinfo");
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

	//string user_head = "(您)  " + strTime + ":\r\n";
	wstring w_user_head = my_userid + L"(您)  " +utf82wstring(strTime)+L":\r\n";

	//清空输入文本框
	SendMessage(SHall->Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	//发送消息到文本框
	SendMessage(SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

	//发送消息
	Message::Hall_Message_Request Req;
	Req.set_content(wstring2utf8(w_content));
	Send(Req);
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
			Return_Class(pinfo);
			delete pinfo;
		}
		lck.unlock();
	}
	return 0;
}

DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam) {
	thread T2(Process_Thread);
	HANDLE hIOCP = (HANDLE)lpParam;
	DWORD dwBytesTranfered = 0;
	unsigned long long pPerHandle;
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
			socket_messageinfo* pinfo = new socket_messageinfo(pPerIO->buf);
			if (sizeof(Header) + pinfo->header.length == dwBytesTranfered)
			{
				unique_lock<mutex> qlck(messagequeue_mutex);
				message_queue.push(pinfo);
				qlck.unlock();
				process_cv.notify_one();
			}
			else {
				delete pinfo;
			}

			/* 重新投递重叠WSARecv请求 */
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
	send_string("CreateRoom");
	host = true;
	Set_CurScene(STATUS::Room_Status);
	Get_Room_Info();
}

void Enter_Room(int index) {
	Message::Hall_EnterRoom_Request Req;
	Req.set_room_id(room_id[index]);
	Send(Req);
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


	int flag = 1;
	setsockopt(mysocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));

	if (-1 == connect(mysocket, (struct sockaddr*)&addr_info, sizeof(struct sockaddr)))
	{
		MessageBoxW(_hwnd, L"无法连接服务器，请检查网络设置", L"网络连接错误", NULL);
		return false;
	}
	unsigned long ul = 1;
	ioctlsocket(mysocket, FIONBIO, &ul);

	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort((HANDLE)mysocket, hIOCP, NULL, 0);
	pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
	pPerIO->nOperationType = OP_READ;
	WSABUF buf;
	buf.buf = pPerIO->buf;
	buf.len = 1023;
	DWORD dwRecv;
	DWORD dwFlags = 0;
	WSARecv(mysocket, &buf, 1, &dwRecv, &dwFlags, &pPerIO->ol, NULL);
	Process_Stop = false;
	thread T1(Recv_Thread, pPerIO, LPVOID(hIOCP));
	T1.detach();
	return true;
}

void Ping_Count(Header& header, char* content)
{
	Message::Ping_info Res;
	Res.ParseFromArray(content, header.length);

	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch()
		);
	int recv_time = ms.count();
	try
	{
		int recv_id = Res.ping_id();
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

	Message::Ping_info info;
	info.set_ping_id(ping_id);
	Send(info);

	if (ping_id != INT32_MAX)
	{
		ping_id++;
		return;
	}

	ping_id = 0;
	while (!ping_queue.empty())
	{
		ping_queue.pop();
	}
}

void set_my_userid()
{
	Message::Set_User_id info;
	info.set_name(wstring2utf8(my_userid));
	Send(info);
}

void Get_Room_Info()
{
	send_string("GetRoominfo");
}

void Return_Get_Roominfo(Header& header, char* content)
{
	(int)SendMessage(SRoom_host->Room_user_list, LB_RESETCONTENT, 0, 0);

	wstring wtemp = my_userid + L"(您)";
	if (host)
	{
		wtemp += L"（房主）";
	}
	else if (isready)
	{
		wtemp += L"（已准备）";
	}
	(int)SendMessage(SRoom_host->Room_user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));

	{
		Message::Room_info_Response Res;
		Res.ParseFromArray(content, header.length);
		for (int i = 0; i < Res.userinfo_size(); i++)
		{
			Message::Room_info_Response_User info = Res.userinfo(i);
			wstring name = utf82wstring(info.name());
			int status = info.status();
			if (status == 1)
			{
				name += L"（已准备）";
			}
			else if (status == 2)
			{
				name += L"（房主）";
			}
			(int)SendMessage(SRoom_host->Room_user_list, LB_ADDSTRING, 0, (LPARAM) & (name[0]));
		}
	}
}

void Room_Ready()
{
	send_string("Ready");
	isready = true;
	SRoom_nothost->ModifyButton_ID(IDB_READY, IDB_CANCELREADY);
	SRoom_nothost->ModifyText_byButton(IDB_CANCELREADY, L"取消准备");
}

void Room_CancelReady()
{
	send_string("CancelReady");
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

	wstring w_user_head = my_userid + L"(您)  " + utf82wstring(strTime) + L":\r\n";
	//清空输入文本框
	SendMessage(Scene_Room::Room_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	//发送消息到文本框
	SendMessage(Scene_Room::edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(Scene_Room::edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(Scene_Room::edit_room, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

	//发送消息
	string i = wstring2utf8(w_content);
	wstring wi = utf82wstring(i);
	string n = "白月光在照耀";
	Message::Room_Message_Request Req;
	Req.set_content(wstring2utf8(w_content));
	Send(Req);
}

void Return_Room_Message(Header& header, char* content) {
	Message::Room_Message_Response Res;
	Res.ParseFromArray(content, header.length);

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
	wstring w_user_head = utf82wstring(Res.name()) + L"  " + utf82wstring(strTime) + L":\r\n";

	//消息内容
	string message_content = Res.content();
	wstring w_message_content = utf82wstring(message_content);

	//发送消息到文本框
	SendMessage(Scene_Room::edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(Scene_Room::edit_room, EM_REPLACESEL, FALSE, (LPARAM) & (w_message_content[0]));
	SendMessage(Scene_Room::edit_room, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

void Return_Set_tankid(Header& header, char* content)
{
	Message::Room_Set_tankid_Response Res;
	Res.ParseFromArray(content, header.length);
	int id = Res.id();
	set_My_id(id);
}

void Return_Start(Header& header, char* content)
{
	static clock_t last_popup = 0;	//记录弹窗时间，避免频繁弹窗
	static clock_t now_popup = 0;
	Message::Room_Start_Response Res;
	Res.ParseFromArray(content, header.length);
	switch (Res.result())
	{
	case 1:
		SendMessage(_hwnd, WM_COMMAND, START, (LPARAM)_hwnd);
		break;
	case 0:
		now_popup = clock();
		if (now_popup - last_popup < 1000)
			return;
		MessageBox(_hwnd, L"有玩家尚未准备！", NULL, MB_OK);
		last_popup = now_popup;
		break;
	case -1:
		now_popup = clock();
		if (now_popup - last_popup < 1000)
			return;
		MessageBox(_hwnd, L"房间人数不足以开始游戏！", NULL, MB_OK);
		last_popup = now_popup;
		break;
	}
}

void win_game()
{
	SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
}

void lost_game()
{
	SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
}