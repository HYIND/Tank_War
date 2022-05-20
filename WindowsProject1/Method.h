#pragma once
#include"Tank.h"
#include <signal.h>
#include <string>
#include <regex>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define IDB_ONE     3301  
#define IDB_TWO     3302  
#define IDB_THREE   3303  
#define IDB_FOUR	3304  
#define IDB_FIVE	3305
#define IDB_SIX		3306
#define IDB_SEVEN	3307

void START(HWND hWnd);
void Get_Init_UI(HWND hWnd);
void Show_Main_UI();
void Hide_Main_UI();
void Return_To_Mune();
void SIG_IO();
void Get_Hallinfo(HWND& room_list, HWND& user_list);
wstring string2wstring(string str);
string wstring2string(wstring wstr);
bool isconnecting();
void Send_Message(wstring& ws);
void recv_socket();