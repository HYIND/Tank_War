#pragma once
//#include "Tank.h"
//#include <signal.h>
//#include <regex>
//#include <thread>
#include "header.h"
#include "Scene.h"

using namespace std;

//hIOCP 重叠结构体
typedef struct _PER_IO_DATA
{
	OVERLAPPED ol;            //重叠结构
	char buf[1024];    //数据缓冲区
	int nOperationType;       //I/O操作类型
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

struct Header
{
	int type = 0;
	int length = 0;
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


//按钮、位图及事件标识符


#define IDB_Bitmap1 3401
#define IDB_Bitmap2 3402

#define Enterroom 6666
#define START 7777
#define DISBANDINROOM 8888
#define DISBANDINEND 9999
#define WIN 666
#define FAIL 777

extern wstring my_userid;

extern queue<Ping_info> ping_queue;
extern int delay;

extern SOCKET tcp_socket;

void SIG_IO();

bool isconnecting();
void send_string(string s);

void Get_Hallinfo();
void Send_Hall_Message(wstring& w_content);
void Create_Room();
void Enter_Room(int index);

void Get_Room_Info();
void Send_Room_Message(wstring& w_content);

void win_game();
void lost_game();

void send_pingmessage();

void Return_Class(socket_messageinfo info);

void Ping_Count(Header& header, char* content);

void Return_Get_Hallinfo(Header& header, char* content);
void Return_Hall_Message(Header& header, char* content);
void Return_Enter_Room(Header& header, char* content);

void Return_Get_Roominfo(Header& header, char* content);
void Return_Room_Message(Header& header, char* content);
void Return_Set_tankid(Header& header, char* content);
void Return_Start(Header& header, char* content);


DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam);
DWORD WINAPI Process_Thread();

void Show_Hall(bool flag);
void Show_Room(bool flag);

bool Init_Hall();

void set_my_userid();

void Room_Ready();
void Room_CancelReady();
void Set_CurScene(STATUS status_in);

void close_connect();

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
