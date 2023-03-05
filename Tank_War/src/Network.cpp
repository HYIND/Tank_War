#include"Network.h"
#include"Game.h"

extern STATUS status;

extern HWND _hwnd;

NetManager* NetManager::Instance()
{
	static NetManager* m_Instance = new NetManager();
	return m_Instance;
}

int NetManager::Get_Header_Type_bystring(string& str)
{
	if (str == "GetHallinfo")return 101;
	else if (str == "CreateRoom") return 104;
	else if (str == "GetRoominfo") return 110;
	else if (str == "Ready") return 111;
	else if (str == "CancelReady") return 112;
	else if (str == "StartGame")return 115;
	else if (str == "QuitRoom") return 116;
}

void NetManager::send_string(string s) {
	Header header;
	header.type = Get_Header_Type_bystring(s);
	if (header.type == 0)
		return;
	header.length = 0;
	char send_buf[sizeof(Header)] = { '\0' };
	memcpy(send_buf, &header, sizeof(Header));
	send(tcp_socket, send_buf, sizeof(Header), 0);
}

string NetManager::get_local_ip()
{
	//查找主机名		
	char host_name[256];
	gethostname(host_name, 256);

	struct addrinfo* result;
	struct addrinfo* ptr;
	struct sockaddr_in* sockaddr_ipv4;
	//根据主机名获得主机信息
	if (getaddrinfo(host_name, NULL, NULL, &result) != 0)
		return "";
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if (ptr->ai_family == AF_INET) {
			sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
			//struct in_addr addr;
			//addr.S_un.S_addr = (*(u_long*)remoteHost->h_addr_list[0]);
			WCHAR w_ip[20] = TEXT("0");
			if (InetNtop(AF_INET, &ptr->ai_addr->sa_data, w_ip, 20) == NULL)
				return "";

			string ip = wstring2utf8(w_ip);
			return ip;
		}
	}
	return "";
}

void NetManager::send_udp_info()
{
	if (udp_socket == INVALID_SOCKET) {
		//string local_ip = get_local_ip();
		udp_socket = get_new_socket("", 0, SOCK_DGRAM, udp_local_addr);	//指定端口为0，bind时由系统自动分配端口

		//if (bind(udp_socket, (struct sockaddr*)&udp_local_addr, sizeof(struct sockaddr)) != 0)
		//{
		//	udp_socket = INVALID_SOCKET;
		//	return;
		//}
	}
	if (-1 == connect(udp_socket, (struct sockaddr*)&udp_remote_addr, sizeof(struct sockaddr)))
		return;

	Message::UDP_INFO_REQ Req;
	Req.set_ip("0.0.0.0");
	Req.set_port(ntohs(udp_local_addr.sin_port));

	Send(Req);
}

void NetManager::recv_udp_info(Header& header, char* content)
{
	Message::UDP_INFO_REQ Req;
	Req.ParseFromArray(content, header.length);

	if (Req.ip() == "0.0.0.0")
		udp_remote_addr.sin_addr.s_addr = tcp_remote_addr.sin_addr.s_addr;
	else
		inet_pton(AF_INET, Req.ip().c_str(), &(udp_remote_addr.sin_addr.s_addr));
	udp_remote_addr.sin_port = htons(Req.port());

	send_udp_info();

	char buf[100];
	recv(udp_socket, buf, 50, 0);
}

void NetManager::Return_Class(socket_messageinfo* info)
{
	if (info->header.type == 206)
	{
		Ping_Count(info->header, info->content);
		return;
	}
	if (info->header.type == 800)
	{
		recv_udp_info(info->header, info->content);
		return;
	}
	if (info->header.type == 801)
	{
		Message::UDP_INFO_RES Res;
		Res.ParseFromArray(info->content, info->header.length);
		if (Res.ok() == true)
			udp_states = true;
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
			Game::Instance()->refreash_tankinfo(info->header, info->content);
			break;
		case 221:
			Game::Instance()->refreash_bullet(info->header, info->content);
			break;
		case 222:
			Game::Instance()->recv_hitbrick(info->header, info->content);
			break;
		case 223:
			Game::Instance()->recv_hited(info->header, info->content);
			break;
		case 224:
			Game::Instance()->recv_myhited();
			break;
		case 225:
			Game::Instance()->recv_destoryed(info->header, info->content);
			break;
		case 226:
			Game::Instance()->recv_mydestoryed();
			break;
		case 227:
			SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
			break;
		case 228:
			SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
			break;
		case 216:
			SendMessage(_hwnd, WM_COMMAND, DISBANDINEND, (LPARAM)_hwnd);
			break;
		}
	}
	break;
	}
}

void NetManager::Return_Get_Hallinfo(Header& header, char* content) {
	Message::Hall_info_Response Res;
	Res.ParseFromArray(content, header.length);

	{
		(int)SendMessage(_Scene::SHall->Hall_user_list, LB_RESETCONTENT, 0, 0);
		wstring w_name = my_userid + L"(您)";
		(int)SendMessage(_Scene::SHall->Hall_user_list, LB_ADDSTRING, 0, (LPARAM) & (w_name[0]));
		string name;
		for (int i = 0; i < Res.userinfo_size(); i++) {
			Message::Hall_info_Response_User userinfo = Res.userinfo(i);
			name = userinfo.name();
			w_name = utf82wstring(name);
			(int)SendMessage(_Scene::SHall->Hall_user_list, LB_ADDSTRING, 0, (LPARAM) & (w_name[0]));
		}
	}

	{
		(int)SendMessage(_Scene::SHall->Hall_room_list, LB_RESETCONTENT, 0, 0);

		string name;
		wstring w_name;
		int counter = 0;
		for (int i = 0; i < Res.roominfo_size(); i++)
		{
			Message::Hall_info_Response_Roominfo roominfo = Res.roominfo(i);
			int id = roominfo.room_id();
			name = roominfo.host_name();
			w_name = utf82wstring(name) + L"的房间";
			room_id[counter] = id;
			counter++;
			(int)SendMessage(_Scene::SHall->Hall_room_list, LB_ADDSTRING, 0, (LPARAM) & (w_name[0]));
		}
	}
}

void NetManager::Return_Hall_Message(Header& header, char* content) {
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
	SendMessage(_Scene::SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_message_head[0]));
	SendMessage(_Scene::SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_message_content[0]));
	SendMessage(_Scene::SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

void NetManager::Return_Enter_Room(Header& header, char* content) {
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

void NetManager::Get_Hallinfo() {
	static clock_t last_refreash = 0;
	static clock_t now_refreash = 0;
	now_refreash = clock();
	if (now_refreash - last_refreash <= 1000)
		return;
	last_refreash = now_refreash;
	send_string("GetHallinfo");
}

void NetManager::Send_Hall_Message(wstring& w_content) {
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
	wstring w_user_head = my_userid + L"(您)  " + utf82wstring(strTime) + L":\r\n";

	//清空输入文本框
	SendMessage(_Scene::SHall->Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
	//发送消息到文本框
	SendMessage(_Scene::SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_user_head[0]));
	SendMessage(_Scene::SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM) & (w_content[0]));
	SendMessage(_Scene::SHall->edit_hall, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

	//发送消息
	Message::Hall_Message_Request Req;
	Req.set_content(wstring2utf8(w_content));
	Send(Req);
}

DWORD WINAPI NetManager::Process_Thread() {
	while (!Process_Stop)
	{
		unique_lock<mutex> lck(process_mtx);
		process_cv.wait(lck);
		while (!message_queue.empty())
		{
			unique_lock<mutex> qlck(messagequeue_mutex);
			socket_messageinfo* pinfo = message_queue.front();
			message_queue.pop();
			qlck.release()->unlock();
			Return_Class(pinfo);
			delete pinfo;
		}
	}
	return 0;
}

DWORD WINAPI NetManager::Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam) {
	HANDLE hIOCP = (HANDLE)lpParam;
	DWORD dwBytesTranfered = 0;

	WSABUF buf;
	buf.buf = pPerIO->buf;
	buf.len = 1023;
	pPerIO->nOperationType = OP_READ;
	DWORD nFlags = 0;
	::WSARecv(tcp_socket, &buf, 1, &dwBytesTranfered, &nFlags, &pPerIO->ol, NULL);

	socket_messageinfo* pinfo = nullptr;
	int cur = 0;
	int pack_flag = 0;	// 分包/黏包标志	0:正常 1:需要黏包(黏包起点为header)  2:需要黏包(黏包起点为content)


	//AllocConsole();	//测试用控制台输出
	//HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	unsigned long long pPerHandle;
	while (!Recv_Stop) {
		bool bl = ::GetQueuedCompletionStatus(hIOCP, &dwBytesTranfered, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED*)&pPerIO, WSA_INFINITE);
		if (!bl)
		{
			closesocket(tcp_socket);
			GlobalFree(pPerIO);
			break;
		}
		if (dwBytesTranfered == 0 && (pPerIO->nOperationType == OP_READ || pPerIO->nOperationType == OP_WRITE)) {
			closesocket(tcp_socket);
			GlobalFree(pPerIO);
			break;
		}
		switch (pPerIO->nOperationType)
		{   //通过per-IO数据中的nOperationType域查看有什么I/O请求完成了
		case OP_READ:  //完成一个接收请求
		{
			// dwBytesTranfered表示接收到的字节数，cur表示缓冲区剩余字节数（因为需要黏包而残留在缓冲区中）
			// 新的dwBytesTranfered表示缓冲区总共字节数
			dwBytesTranfered = dwBytesTranfered + cur;
			cur = 0;

			// 粘包标志不为0，表示需要黏包
			if (pack_flag == 1 && dwBytesTranfered - cur >= sizeof(Header))
			{
				pinfo = new socket_messageinfo();
				pinfo->set_header(&(pPerIO->buf[cur]));

				//{	// 测试用控制台输出
				//	wstring wstr = to_wstring(pinfo->header.type) + L"\n";
				//	wchar_t szOutputTest[MAX_PATH];
				//	lstrcpy(szOutputTest, wstr.c_str());
				//	DWORD dwStringLength = wcslen(szOutputTest);
				//	DWORD dwBytesWritten = 0;
				//	DWORD dwErrorCode = 0;
				//	WriteConsole(hStdout, szOutputTest, dwStringLength, &dwBytesWritten, NULL);
				//}

				cur += sizeof(Header);
				pack_flag = 0;
			}
			else if (pack_flag == 2 && dwBytesTranfered - cur >= pinfo->header.length)
			{
				pinfo->set_content(&(pPerIO->buf[cur]));
				cur += pinfo->header.length;
				unique_lock<mutex> qlck(messagequeue_mutex);
				message_queue.push(pinfo);
				qlck.release()->unlock();
				process_cv.notify_one();
				pinfo = nullptr;
				pack_flag = 0;
			}


			while (!pack_flag)
			{
				// pinfo为空则获取包头，若不为空（说明包头在黏包过程中获取了，只需要再获取内容就好了）
				if (!pinfo)
				{
					// 剩余字符已经不够组成新的头部，需要黏包
					if (dwBytesTranfered - cur < sizeof(Header)) {
						// 把剩余字符移动至buf起始位置
						memmove(pPerIO->buf, &(pPerIO->buf[cur]), dwBytesTranfered - cur);
						cur = dwBytesTranfered - cur;
						pack_flag = 1;
						break;
					}
					else {
						//获取头
						pinfo = new socket_messageinfo();
						pinfo->set_header(&(pPerIO->buf[cur]));

						//{		// 测试用控制台输出
						//	wstring wstr = to_wstring(pinfo->header.type) + L"\n";
						//	wchar_t szOutputTest[MAX_PATH];
						//	lstrcpy(szOutputTest, wstr.c_str());
						//	DWORD dwStringLength = wcslen(szOutputTest);
						//	DWORD dwBytesWritten = 0;
						//	DWORD dwErrorCode = 0;
						//	WriteConsole(hStdout, szOutputTest, dwStringLength, &dwBytesWritten, NULL);
						//}

						cur += sizeof(Header);
					}
				}

				// 剩余字符已经不够组成新的头部，需要黏包
				if (dwBytesTranfered - cur < pinfo->header.length)
				{
					// 把剩余字符移动至buf起始位置
					memmove(pPerIO->buf, &(pPerIO->buf[cur]), dwBytesTranfered - cur);
					cur = dwBytesTranfered - cur;
					pack_flag = 2;
					break;
				}
				else {
					pinfo->set_content(&(pPerIO->buf[cur]));
					cur += pinfo->header.length;
					unique_lock<mutex> qlck(messagequeue_mutex);
					message_queue.push(pinfo);
					qlck.release()->unlock();
					process_cv.notify_one();
					pinfo = nullptr;
				}

				// 相等则说明不需要黏包
				if (cur == dwBytesTranfered) {
					cur = 0;
					break;
				}
			}

			/* 重新投递重叠WSARecv请求 */
			WSABUF buf;
			buf.buf = &(pPerIO->buf[cur]);
			buf.len = 1023 - cur;
			pPerIO->nOperationType = OP_READ;
			DWORD nFlags = 0;
			::WSARecv(tcp_socket, &buf, 1, &dwBytesTranfered, &nFlags, &pPerIO->ol, NULL);
		}
		break;
		case OP_WRITE:break;
		}
	}
	WSACleanup();
	return 0;
}

void NetManager::Create_Room() {
	send_string("CreateRoom");
	host = true;
	Set_CurScene(STATUS::Room_Status);
	Get_Room_Info();
}

void NetManager::Enter_Room(int index) {
	Message::Hall_EnterRoom_Request Req;
	Req.set_room_id(room_id[index]);
	Send(Req);
}

bool NetManager::Init_Hall()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	udp_states = false;

	tcp_socket = get_new_socket(SERVER_IP, SERVER_PORT, SOCK_STREAM, tcp_remote_addr);

	int flag = 1;
	setsockopt(tcp_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));

	if (-1 == connect(tcp_socket, (struct sockaddr*)&tcp_remote_addr, sizeof(struct sockaddr)))
	{
		LOGINFO("Connect , Connect Server Failed!");
		MessageBoxW(_hwnd, L"无法连接服务器，请检查网络设置", L"网络连接错误", NULL);
		return false;
	}
	unsigned long ul = 1;
	ioctlsocket(tcp_socket, FIONBIO, &ul);

	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort((HANDLE)tcp_socket, hIOCP, NULL, NumberOfRecvThread);

	Process_Stop = false;
	thread T1(&NetManager::Process_Thread, this);
	T1.detach();

	Recv_Stop = false;
	//for (int i = 0; i < NumberOfRecvThread; i++)
	//{
	//	PPER_IO_DATA pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
	//	pPerIO->nOperationType = OP_READ;
	//	thread T2(Recv_Thread, pPerIO, LPVOID(hIOCP));
	//	T2.detach();
	//}

	PPER_IO_DATA pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
	pPerIO->nOperationType = OP_READ;
	thread T2(&NetManager::Recv_Thread, this, pPerIO, LPVOID(hIOCP));
	T2.detach();

	LOGINFO("Connect , Connect Server Success!  userid: {}", wstring2utf8(my_userid));
	return true;
}

void NetManager::Ping_Count(Header& header, char* content)
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

void NetManager::send_pingmessage()
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

void NetManager::Send_my_userid()
{
	Message::Set_User_id info;
	info.set_name(wstring2utf8(my_userid));
	Send(info);
}

void NetManager::Get_Room_Info()
{
	send_string("GetRoominfo");
}

void NetManager::Return_Get_Roominfo(Header& header, char* content)
{
	(int)SendMessage(_Scene::SRoom_host->Room_user_list, LB_RESETCONTENT, 0, 0);

	wstring wtemp = my_userid + L"(您)";
	if (host)
	{
		wtemp += L"（房主）";
	}
	else if (isready)
	{
		wtemp += L"（已准备）";
	}
	(int)SendMessage(_Scene::SRoom_host->Room_user_list, LB_ADDSTRING, 0, (LPARAM) & (wtemp[0]));

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
			(int)SendMessage(_Scene::SRoom_host->Room_user_list, LB_ADDSTRING, 0, (LPARAM) & (name[0]));
		}
	}
}

void NetManager::Room_Ready()
{
	send_string("Ready");
	isready = true;
	_Scene::SRoom_nothost->ModifyButton_ID(IDB_READY, IDB_CANCELREADY);
	_Scene::SRoom_nothost->ModifyText_byButton(IDB_CANCELREADY, L"取消准备");
}

void NetManager::Room_CancelReady()
{
	send_string("CancelReady");
	isready = false;
	_Scene::SRoom_nothost->ModifyButton_ID(IDB_CANCELREADY, IDB_READY);
	_Scene::SRoom_nothost->ModifyText_byButton(IDB_READY, L"准备");
}

void NetManager::Send_Room_Message(wstring& w_content) {
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
	Message::Room_Message_Request Req;
	Req.set_content(wstring2utf8(w_content));
	Send(Req);
}

void NetManager::Return_Room_Message(Header& header, char* content) {
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

void NetManager::Return_Set_tankid(Header& header, char* content)
{
	Message::Room_Set_tankid_Response Res;
	Res.ParseFromArray(content, header.length);
	int id = Res.id();
	set_My_id(id);
}

void NetManager::Return_Start(Header& header, char* content)
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

void NetManager::win_game()
{
	SendMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
}

void NetManager::lost_game()
{
	SendMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
}

void NetManager::close_connect()
{
	Recv_Stop = true;
	Process_Stop = true;
	process_cv.notify_one();
	closesocket(tcp_socket);
}