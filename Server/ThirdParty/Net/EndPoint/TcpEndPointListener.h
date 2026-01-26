#pragma once

#include "Connection/TCPTransportListener.h"
#include "EndPoint/TCPEndPoint.h"
#include "Timer.h"

struct ClientData;

// 用于监听指定协议的TCP连接，用于校验连接协议
class NET_API TcpEndPointListener
{
public:
	TcpEndPointListener(TCPNetProtocol proto = TCPNetProtocol::PureTCP);
	~TcpEndPointListener();

	TCPNetProtocol Protocol();
	void SetProtocol(const TCPNetProtocol& proto);
	bool Listen(const std::string& IP, int Port);
	void BindEstablishConnectionCallBack(std::function<void(TCPEndPoint*)> callback);

private:
	void RecvCon(std::shared_ptr<TCPTransportConnection> waitCon);
	void ConClose(TCPTransportConnection* Con);
	void Handshake(TCPTransportConnection* waitCon, Buffer* buf);
	void CleanExpiredClient();

private:
	std::shared_ptr<TCPTransportListener> BaseListener;
	TCPNetProtocol _Protocol;
	std::function<void(TCPEndPoint*)> _callBackEstablish;
	SafeArray<std::shared_ptr<ClientData>> waitClients; // 等待校验协议的客户端
#ifdef __linux__
	std::shared_ptr<TimerTask> CleanExpiredTask;
#endif
};
