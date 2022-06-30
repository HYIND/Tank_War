#pragma once
#define WIN32_LEAN_AND_MEAN

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
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib,"Dwrite.lib")

//D2D释放资源
#define SafeRelease(P) if(P){P->Release() ; P = NULL ;}

#include <vector>
#include <string>
#include <queue>
#include <mutex> 