#pragma once

#include "EndPoint/WebSocketClient.h"
#include "Session/BaseNetWorkSession.h"

struct PureWebSocketSessionPakage
{
	Buffer buffer;
};

class NET_API PureWebSocketSession : public BaseNetWorkSession
{

public:
	PureWebSocketSession(WebSocketClient* client = nullptr);
	~PureWebSocketSession();
	virtual bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	virtual Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	virtual bool Release();

	virtual bool AsyncSend(const Buffer& buffer); // 异步发送，不关心返回结果

	WebSocketClient* GetBaseClient();

public:
	virtual bool TryHandshake(uint32_t timeOutMs);
#ifdef __linux__
	virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs);
#endif
	virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer& buffer);
	virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer& buffer);

protected:
	virtual bool OnSessionClose();
	virtual bool OnRecvData(Buffer* buffer);
	virtual void OnBindRecvDataCallBack();
	virtual void OnBindSessionCloseCallBack();

private:
	bool Send(const Buffer& buffer);
	void ProcessPakage(PureWebSocketSessionPakage* newPak = nullptr);
	SpinLock _ProcessLock;

private:
	SafeQueue<PureWebSocketSessionPakage*> _RecvPaks;
};
