
/*
	该文件是对TCP传输层协议下的基础连接对象的封装
	其中包含一个TCP连接监听器，TCP连接客户端
 */

#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "fmt/core.h"
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
#include "Coroutine.h"
#include "SpinLock.h"
#include "DeleteLater.h"

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

class BaseTransportConnection : public std::enable_shared_from_this<BaseTransportConnection>, public DeleteLaterImpl
{

public:
	template <typename T>
	std::shared_ptr<T> GetShared()
	{
		if (auto ptr = dynamic_cast<T *>(this))
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

	EXPORT_FUNC int GetFd();
	EXPORT_FUNC SocketType GetType();
	EXPORT_FUNC sockaddr_in GetAddr();
	EXPORT_FUNC char *GetIPAddr();
	EXPORT_FUNC uint16_t GetPort();
	EXPORT_FUNC NetType GetNetType();
	EXPORT_FUNC bool ValidSocket();

	EXPORT_FUNC bool isOnCallback();

public:
	EXPORT_FUNC void RDHUP();
	EXPORT_FUNC void READ(int fd);
	EXPORT_FUNC void READ(int fd, Buffer &buf);
	EXPORT_FUNC void ACCEPT(int fd);
	EXPORT_FUNC void ACCEPT(int fd, int newclient, sockaddr_in addr);

protected:
	EXPORT_FUNC virtual void OnRDHUP() = 0;											// 对端关闭事件，即断开连接
	EXPORT_FUNC virtual void OnREAD(int fd) = 0;									// 可读事件
	EXPORT_FUNC virtual void OnREAD(int fd, Buffer &buf) = 0;						// 可读事件
	EXPORT_FUNC virtual void OnACCEPT(int fd) = 0;									// 接受新连接事件
	EXPORT_FUNC virtual void OnACCEPT(int fd, int newclient, sockaddr_in addr) = 0; // 接受新连接事件

protected:
	sockaddr_in _addr;
	int _fd = -1;
	SocketType _type = SocketType::TCP;
	bool _isclient;

	std::atomic<int> _OnRDHUPCount;
	std::atomic<int> _OnREADCount;
	std::atomic<int> _OnACCEPTCount;
};

// TCP传输层客户端(连接对象)
class TCPTransportConnection : public BaseTransportConnection
{

public:
	EXPORT_FUNC TCPTransportConnection();
	EXPORT_FUNC ~TCPTransportConnection();
	EXPORT_FUNC bool Connect(const std::string &IP, uint16_t Port);
	EXPORT_FUNC Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
	EXPORT_FUNC void Apply(const int fd, const sockaddr_in &sockaddr, const SocketType type);
	EXPORT_FUNC bool Release();
	EXPORT_FUNC bool Send(const Buffer &buffer);
	EXPORT_FUNC int Read(Buffer &buffer, int length);

	EXPORT_FUNC void BindBufferCallBack(std::function<void(TCPTransportConnection *, Buffer *)> callback);
	EXPORT_FUNC void BindRDHUPCallBack(std::function<void(TCPTransportConnection *)> callback);

	EXPORT_FUNC SafeQueue<Buffer *> &GetRecvData();
	EXPORT_FUNC SafeQueue<Buffer *> &GetSendData();
	EXPORT_FUNC CriticalSectionLock &GetSendMtx();

protected:
	EXPORT_FUNC virtual void OnRDHUP();
	EXPORT_FUNC virtual void OnREAD(int fd);									// 可读事件
	EXPORT_FUNC virtual void OnREAD(int fd, Buffer &buf);						// 可读事件
	EXPORT_FUNC virtual void OnACCEPT(int fd);									// 接受新连接事件
	EXPORT_FUNC virtual void OnACCEPT(int fd, int newclient, sockaddr_in addr); // 接受新连接事件

protected:
	EXPORT_FUNC virtual void OnBindBufferCallBack();
	EXPORT_FUNC virtual void OnBindRDHUPCallBack();

private:
	void ProcessRecvQueue();

private:
	SafeQueue<Buffer *> _RecvDatas;
	SafeQueue<Buffer *> _SendDatas;

private:
	std::function<void(TCPTransportConnection *, Buffer *)> _callbackBuffer;
	std::function<void(TCPTransportConnection *)> _callbackRDHUP;
	CriticalSectionLock _SendResMtx;
	SpinLock _ProcessLock;
};

// TCP传输层监听器
class TCPTransportListener : public BaseTransportConnection
{

public:
	EXPORT_FUNC TCPTransportListener();
	EXPORT_FUNC ~TCPTransportListener();
	EXPORT_FUNC bool Listen(const std::string &IP, int Port);
	EXPORT_FUNC bool ReleaseListener();
	EXPORT_FUNC bool ReleaseClients();
	EXPORT_FUNC void BindAcceptCallBack(std::function<void(std::shared_ptr<TCPTransportConnection>)> callback);

public:
	EXPORT_FUNC virtual void OnRDHUP();
	EXPORT_FUNC virtual void OnREAD(int fd);									// 可读事件
	EXPORT_FUNC virtual void OnREAD(int fd, Buffer &buf);						// 可读事件
	EXPORT_FUNC virtual void OnACCEPT(int fd);									// 接受新连接事件
	EXPORT_FUNC virtual void OnACCEPT(int fd, int newclient, sockaddr_in addr); // 接受新连接事件

private:
	std::function<void(std::shared_ptr<TCPTransportConnection>)> _callbackAccept;
};
