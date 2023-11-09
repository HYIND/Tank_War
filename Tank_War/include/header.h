#pragma once
#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"

#include "Tank_War.h"
#include "framework.h"
#include <comdef.h>
// WinSocket
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib,"Kr")
// 
//#pragma comment(lib,"dxguid.lib")

#include <signal.h>
#include <regex>
#include <thread>

//d2d及位图 头文件
//#include <d2d1.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <wincodec.h>
#include <d2d1_3helper.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib,"Dwrite.lib")

//D2D释放资源
#define SafeRelease(P) if(P){P->Release() ; P = NULL ;}

#include <vector>
#include <string>
#include <queue>
#include <mutex> 

#include <set>
#include <map>
#include <unordered_map>
#include <timeapi.h>
#pragma comment(lib,"Winmm.lib")

#include "myprotocol.pb.h"
#include <type_traits>

#define LOGGERMODE_ON
#include "Log.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

extern bool isstart;
extern bool isready;
extern bool host;
extern bool isonline_game;


//wstring stringh2wstring(string str);
//string wstring2string(wstring wstr);

string wstring2utf8(const std::wstring& str);
wstring utf82wstring(const std::string& str);
SOCKET get_new_socket(std::string IP, uint16_t socket_port, int protocol, sockaddr_in& addr);

