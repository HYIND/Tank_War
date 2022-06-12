#pragma once
#include "Tank.h"
#include <signal.h>
#include <string>
#include <regex>
#include <thread>


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


//按钮、位图及事件标识符
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
#define QUITROOM 9999

//status 枚举
enum { NONE, Hall_Status, Room_Status, Game_Status };
//D2D释放资源
#define SafeRelease(P) if(P){P->Release() ; P = NULL ;}

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
void Send_Message(wstring& w_content);
void Return_Class(char buf[]);
DWORD WINAPI Recv_Thread(PPER_IO_DATA pPerIO, LPVOID lpParam);
void Create_Room();
void Enter_Room(int index);
void send_location(Tank* tank);
void send_bullet(bullet* bullet_head);
void Refresh_opTank(char buf[]);
void Refresh_opbullet(string& re);
void send_destroy(bullet* bullet);

HRESULT Loadbitmap(IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget, LPCTSTR pszResource, ID2D1Bitmap** ppBitmap);
