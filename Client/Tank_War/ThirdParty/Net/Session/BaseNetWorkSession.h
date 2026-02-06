#pragma once

#include "EndPoint/TCPEndPoint.h"
#include "Core/DeleteLater.h"
#ifdef __linux__
#include "Coroutine.h"
#endif
#include "CriticalSectionLock.h"

class NET_API BaseNetWorkSession : public DeleteLaterImpl
{

public:
	BaseNetWorkSession();
	virtual ~BaseNetWorkSession();
	virtual bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	virtual Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	virtual bool Release();

public: // 供Listener/EndPoint调用,须继承实现
	virtual bool AsyncSend(const Buffer& buffer) = 0;
	virtual bool TryHandshake(uint32_t timeOutMs) = 0;
#ifdef __linux__
	virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs) = 0;
#endif
	virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer& buffer) = 0;
	virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer& buffer) = 0;

public: // 供外部调用
	void BindRecvDataCallBack(std::function<void(BaseNetWorkSession*, Buffer* recv)> callback);
	void BindSessionCloseCallBack(std::function<void(BaseNetWorkSession*)> callback);
	char* GetIPAddr();
	uint16_t GetPort();

public: // 供Listener/EndPoint调用
	void RecvData(TCPEndPoint* client, Buffer* buffer);
	void SessionClose(TCPEndPoint* client);
	TCPEndPoint* GetBaseClient();

protected: // 须继承实现
	virtual bool OnSessionClose() = 0;
	virtual bool OnRecvData(Buffer* buffer) = 0;
	virtual void OnBindRecvDataCallBack() = 0;
	virtual void OnBindSessionCloseCallBack() = 0;

protected:
	TCPEndPoint* BaseClient;

	bool isHandshakeComplete;

	std::function<void(BaseNetWorkSession*, Buffer* recv)> _callbackRecvData;
	std::function<void(BaseNetWorkSession*)> _callbackSessionClose;

#ifdef __linux__
	CoTimer* _handshaketimeout;
	CriticalSectionLock _Colock;
#endif
};
