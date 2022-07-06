#pragma once
//#include "Tank.h"
//#include <signal.h>
//#include <regex>
//#include <thread>
#include "header.h"
#include"D2D_Scene.h"

using namespace std;

//hIOCP �ص��ṹ��
typedef struct _PER_IO_DATA
{
	OVERLAPPED ol;            //�ص��ṹ
	char buf[1024];    //���ݻ�����
	int nOperationType;       //I/O��������
#define OP_READ 1
#define OP_WRITE 2
#define OP_ACCEPT 3
}PER_IO_DATA, * PPER_IO_DATA;

//���汾�ط��͵�ping��Ϣ�����Լ����ӳ�(pingֵ)
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


//struct player_info
//{
//
//};
//
//class Room_info
//{
//	vector<>
//};


//��ť��λͼ���¼���ʶ��


#define IDB_Bitmap1 3401
#define IDB_Bitmap2 3402

#define Enterroom 7777
#define START 8888
#define QUITROOM 9999
#define WIN 666
#define FAIL 777

#define DEFAULT_PORT 2336
#define SERVER_IP "175.178.90.119"


extern queue<Ping_info> ping_queue;
extern int delay;


//status ö��
enum class STATUS { Main, Option, Hall_Status, Room_Status, Game_Status };

//void START(HWND hWnd);
//void Get_Init_UI(HWND hWnd);
void Show_Main_UI();
void Hide_Main_UI();
void Return_To_Mune();
void SIG_IO();
wstring string2wstring(string str);
string wstring2string(wstring wstr);

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
void ReturnToRoom();

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


HRESULT Loadbitmap(IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget, LPCTSTR pszResource, ID2D1Bitmap** ppBitmap);
HRESULT LoadResourceBitmap(HINSTANCE hinstance, IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR resourceType, LPCWSTR resourceName, ID2D1Bitmap** ppBitmap);
HBRUSH OnCtlColorEdit(WPARAM wParam, LPARAM lParam);

void Show_Hall(bool flag);
void Show_Room(bool flag);

bool Init_Hall();

void setmyuserid();

void Room_Ready();
void Room_CancelReady();
void Set_CurScene(STATUS status_in);