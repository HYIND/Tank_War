#pragma once

#include "EndPoint/TcpEndPointListener.h"
#include "Session/BaseNetWorkSession.h"
#include "Timer.h"

enum class SessionType
{
	None = 0,
	CustomTCPSession,
	CustomWebSockectSession,
	PureWebSocketSession
};

struct SessionData;

// 用于接受自定义通讯协议会话，在接受客户端（Client）的基础上，根据通讯协议执行握手行为
class NET_API NetWorkSessionListener
{
public:
	NetWorkSessionListener(SessionType type);
	~NetWorkSessionListener();

	bool Listen(const std::string& IP, int Port);
	void BindSessionEstablishCallBack(std::function<void(BaseNetWorkSession*)> callback);

private:
	void RecvClient(TCPEndPoint* client);
	void ClientClose(TCPEndPoint* client);
	void Handshake(TCPEndPoint* waitClient, Buffer* buf);
	void CleanExpiredSession();

private:
	SessionType _sessiontype;
	TcpEndPointListener BaseListener;
	std::function<void(BaseNetWorkSession*)> _callBackSessionEstablish;
	SafeArray<std::shared_ptr<SessionData>> waitSessions; // 等待校验协议的客户端
#ifdef __linux__
	std::shared_ptr<TimerTask> CleanExpiredTask;
#endif
};