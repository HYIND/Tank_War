#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//#include <WinSock2.h>
//#include <MSWSock.h>
//#include <WS2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "mswsock.lib")

#include <Windows.h>

#ifdef _DEBUG
#pragma comment(lib,"public_d.lib")
#pragma comment(lib,"net_d.lib")
#pragma comment(lib,"box2d_d.lib")
#else
#pragma comment(lib,"public.lib")
#pragma comment(lib,"net.lib")
#pragma comment(lib,"box2d.lib")
#endif

#include <iostream>
#include <string.h>
#include <chrono>
#include <random>
#include <sstream>
#include <string>
#include "Helper/Tools.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;