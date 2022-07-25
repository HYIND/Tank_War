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
#define OP_ACCEPT 3
}PER_IO_DATA, * PPER_IO_DATA;

//保存本地发送的ping信息，用以计算延迟(ping值)
struct Ping_info
{
	int id;
	int send_time;
};

struct socket_messageinfo
{
	char ch[1024];
	socket_messageinfo(char ch[])
	{
		memcpy(this->ch, ch, 1024);
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

#define DEFAULT_PORT 2336
#define SERVER_IP "175.178.90.119"

extern wstring my_userid;

extern queue<Ping_info> ping_queue;
extern int delay;

extern SOCKET mysocket;

void SIG_IO();

bool isconnecting();
void send_socket(string s);

void Get_Hallinfo();
void Send_Hall_Message(wstring& w_content);
void Create_Room();
void Enter_Room(int index);

void Get_Room_Info();
void Send_Room_Message(wstring& w_content);

void win_game();
void lost_game();

void send_pingmessage();

void Return_Get_Hallinfo_User(string& re);
void Return_Get_Hallinfo_Room(string re);
void Return_Get_Hallinfo_Roomid(string re);
void Return_Hallinfo_Message(string& recv_str);
void Return_Get_Room_User(string& re);
void Return_Room_Message(string& recv_str);
void Return_Class(char buf[]);
void Ping_Count(string&);

DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam);
DWORD WINAPI Process_Thread();

void Show_Hall(bool flag);
void Show_Room(bool flag);

bool Init_Hall();

void setmyuserid();

void Room_Ready();
void Room_CancelReady();
void Set_CurScene(STATUS status_in);