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
	virtual Task<bool> Connect(std::string IP, uint16_t Port);

	virtual bool Release();

	virtual bool AsyncSend(const Buffer& buffer); // 异步发送，不关心返回结果

	WebSocketClient* GetBaseClient();

public:
	virtual Task<bool> TryHandshake();

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
