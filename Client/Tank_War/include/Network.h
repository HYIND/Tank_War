#pragma once
#include "header.h"
#include "Scene.h"

using namespace std;

//按钮、位图及事件标识符


#define IDB_Bitmap1 3401
#define IDB_Bitmap2 3402

#define Enterroom 6666
#define START 7777
#define DISBANDINROOM 8888
#define DISBANDINEND 9999
#define WIN 666
#define FAIL 777

struct Header
{
	int type = 0;
	int length = 0;
};

#define SERVER_PORT 2336
#define SERVER_IP "175.178.90.119"

#define NumberOfRecvThread 8
class NetManager
{
	//hIOCP 重叠结构体
	typedef struct _PER_IO_DATA
	{
		OVERLAPPED ol;          //重叠结构
		char buf[1024];			//数据缓冲区
		int nOperationType;     //I/O操作类型
#define OP_READ 1
#define OP_WRITE 2
//#define OP_ACCEPT 3
	}PER_IO_DATA, * PPER_IO_DATA;

	//保存本地发送的ping信息，用以计算延迟(ping值)
	struct Ping_info
	{
		int id;
		int send_time;
	};


	struct socket_messageinfo
	{
		Header header;
		char* content = nullptr;
		socket_messageinfo() :content(nullptr) {}

		void set_header(char* ch) {
			memcpy(&header, ch, sizeof(Header));
		}
		void set_content(char* ch) {
			//获取内容（可能为空）
			if (header.length > 0)
			{
				content = new char[header.length];
				memcpy(content, ch, header.length);
			}
		}

	};

private:
	wstring my_userid = L"unname";

	WSADATA wsa;
	HANDLE hIOCP;

	queue<Ping_info> ping_queue;
	int ping_id = 0;
	int delay = 999;

	SOCKET tcp_socket = INVALID_SOCKET;
	sockaddr_in tcp_remote_addr;

	SOCKET udp_socket = INVALID_SOCKET;
	sockaddr_in udp_local_addr;
	sockaddr_in udp_remote_addr;
	bool udp_states = false;

	queue<socket_messageinfo*> message_queue;
	mutex messagequeue_mutex;
	bool Process_Stop = false;
	bool Recv_Stop = false;
	int room_id[65535];

	mutex process_mtx;
	condition_variable process_cv;
public:
	int Get_Delay() { return delay; };

public:
	//void SIG_IO();
	//bool isconnecting();

	static NetManager* Instance();

	bool Init_Hall();
	void Send_my_userid();
	void Set_my_userid(wstring name) { my_userid = name; }
	wstring Get_my_userid() { return my_userid; }
	void send_pingmessage();

	void Get_Hallinfo();
	void Send_Hall_Message(wstring& w_content);
	void Create_Room();
	void Enter_Room(int index);

	void Get_Room_Info();
	void Room_Ready();
	void Room_CancelReady();
	void Send_Room_Message(wstring& w_content);

	void send_string(string s);
	template <typename T>
	void Send(T& message)
	{
		Header header;
		header.type = Get_Header_Type(message);
		if (header.type == 0)
			return;
		header.length = message.ByteSizeLong();

		int len = sizeof(Header) + header.length;
		char* buf = new char[len];
		memset(buf, '\0', len);

		memcpy(buf, &header, sizeof(header));
		message.SerializeToArray(buf + sizeof(Header), header.length);
		send(tcp_socket, buf, len, 0);
		delete(buf);
	}

	void close_connect();

private:
	NetManager() {};
	void Return_Class(socket_messageinfo* info);
	void Ping_Count(Header& header, char* content);
	void Return_Get_Hallinfo(Header& header, char* content);
	void Return_Hall_Message(Header& header, char* content);
	void Return_Enter_Room(Header& header, char* content);

	void Return_Get_Roominfo(Header& header, char* content);
	void Return_Room_Message(Header& header, char* content);
	void Return_Set_tankid(Header& header, char* content);
	void Return_Start(Header& header, char* content);
	void win_game();
	void lost_game();

	DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam);
	DWORD WINAPI Process_Thread();

	int Get_Header_Type_bystring(string& str);

	template <typename T>
	int Get_Header_Type(T& message)
	{
		if (is_same<T, Message::UDP_INFO_REQ>::value)
		{
			return 800;
		}
		else if (is_same<T, Message::Hall_Message_Request>::value)
		{
			return 103;
		}
		else if (is_same<T, Message::Hall_EnterRoom_Request>::value)
		{
			return 105;
		}
		else if (is_same<T, Message::Ping_info>::value)
		{
			return 106;
		}
		else if (is_same<T, Message::Set_User_id>::value)
		{
			return 107;
		}
		else if (is_same<T, Message::Room_Message_Request>::value)
		{
			return 113;
		}
		else if (is_same<T, Message::Game_tankinfo_Request>::value)
		{
			return 120;
		}
		else if (is_same<T, Message::Game_bulletinfo_Request>::value)
		{
			return 121;
		}
		else if (is_same<T, Message::Game_hit_brick_Request>::value)
		{
			return 122;
		}
		else if (is_same<T, Message::Game_hit_tank_Request>::value)
		{
			return 123;
		}
		return 0;
	}



private:	//test
	string get_local_ip();
	void send_udp_info();
	void recv_udp_info(Header& header, char* content);

};

