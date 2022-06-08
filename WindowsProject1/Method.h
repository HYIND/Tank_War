#pragma once
#include"Tank.h"
#include <signal.h>
#include <string>
#include <regex>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib,"Kr")

typedef struct _PER_IO_DATA
{
	OVERLAPPED ol;            //重叠结构
	char buf[1024];    //数据缓冲区
	int nOperationType;       //I/O操作类型
#define OP_READ 1
#define OP_WRITE 2
#define OP_ACCEPT 3
}PER_IO_DATA, * PPER_IO_DATA;

#define IDB_ONE     3301  
#define IDB_TWO     3302  
#define IDB_THREE   3303  
#define IDB_FOUR	3304  
#define IDB_FIVE	3305
#define IDB_SIX		3306
#define IDB_SEVEN	3307

#define IDB_Bitmap1 3401
#define IDB_Bitmap2 3402

#define Enterroom 7777
#define START 8888

//void START(HWND hWnd);
void Get_Init_UI(HWND hWnd);
void Show_Main_UI();
void Hide_Main_UI();
void Return_To_Mune();
void SIG_IO();
void Get_Hallinfo();
void Return_Get_Hallinfo_User(string& re);
void Return_Get_Hallinfo_Room(string re);
void Return_Get_Hallinfo_Roomid(string re);
void Return_Hallinfo_Message(string& recv_str);
wstring string2wstring(string str);
string wstring2string(wstring wstr);
bool isconnecting();
void Send_Message(wstring& ws);
void Return_Class(char buf[]);
DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam);
void Create_Room();
void Enter_Room(int index);
void send_location(Tank* tank);
void send_bullet(bullet* bullet_head);
void Refresh_opTank(char buf[]);
void Refresh_opbullet(string& re);