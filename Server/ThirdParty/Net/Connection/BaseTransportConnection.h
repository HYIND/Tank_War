
/*
	该文件是对TCP传输层协议下的基础连接对象的封装
 */

#pragma once

#include "Core/BaseSocket.h"

#include <atomic>
#include <string.h>
#include <signal.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <functional>
#include <random>
#include <shared_mutex>
#include <map>

#include "Helper/Buffer.h"
#include "SafeStl.h"
#include "SpinLock.h"
#include "Core/DeleteLater.h"

#ifdef __linux__
#include "Coroutine.h"
#endif

enum NetType
{
	Listener = 1,
	Client = 2
};

enum SocketType
{
	TCP = 1,
	UDP = 2
};

class NET_API BaseTransportConnection : public std::enable_shared_from_this<BaseTransportConnection>, public DeleteLaterImpl
{

public:
	template <typename T>
	std::shared_ptr<T> GetShared()
	{
		if (auto ptr = dynamic_cast<T*>(this))
		{
			return std::shared_ptr<T>(
				shared_from_this(),
				ptr);
		}
		return nullptr;
	}
	std::shared_ptr<BaseTransportConnection> GetBaseShared();

public:
	BaseTransportConnection(SocketType type = SocketType::TCP, bool isclient = false);

	BaseSocket GetSocket();
	SocketType GetType();
	sockaddr_in GetAddr();
	char* GetIPAddr();
	uint16_t GetPort();
	NetType GetNetType();
	bool ValidSocket();

	bool isOnCallback();

public:
#ifdef __linux__
	void READ(BaseSocket fd);
	void ACCEPT(BaseSocket fd);
#endif
	void READ(BaseSocket fd, Buffer& buf);
	void ACCEPT(BaseSocket fd, BaseSocket newsocket, sockaddr_in addr);
	void RDHUP();

protected:
#ifdef __linux__
	virtual void OnREAD(BaseSocket socket) = 0;												// 可读事件
	virtual void OnACCEPT(BaseSocket socket) = 0;											// 接受新连接事件
#endif
	virtual void OnREAD(BaseSocket socket, Buffer& buf) = 0;								// 可读事件
	virtual void OnACCEPT(BaseSocket socket, BaseSocket newsocket, sockaddr_in addr) = 0;	// 接受新连接事件
	virtual void OnRDHUP() = 0;																// 对端关闭事件，即断开连接

protected:
	sockaddr_in _addr;
	BaseSocket _socket;
	SocketType _type = SocketType::TCP;
	bool _isclient;

	std::atomic<int> _OnRDHUPCount;
	std::atomic<int> _OnREADCount;
	std::atomic<int> _OnACCEPTCount;
};

