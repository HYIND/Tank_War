#pragma once

#include "SessionManager/CustomTcpSessionManager.h"
#include "JsonProtocolSession.h"
#include "BiDirectionalMap.h"
#include "SafeStl.h"
#include "stdafx.h"

using JsonProtocolSessionID = std::string;

class JsonProtocolServer;

// JsonProtocol会话句柄的轻量级抽象
// 用来访问由JsonProtocolServer进行连接管理的Json会话，不负责生命周期
// 仅用来提供获取Session后主动发送数据或释放连接的功能
class JsonProtocolSession
{

public:
	JsonProtocolSession();
	JsonProtocolSession(const JsonProtocolSessionID& sessionId, JsonProtocolServer* owner_server);

	bool AsyncSendJson(const json& js);
	Task<bool> AwaitSendJson(const json& req, json& resp);

	bool Release();

	JsonProtocolSessionID getsessionId() const;
	JsonProtocolServer* getOwner() const;

	bool operator==(const JsonProtocolSession& other) const;
	bool operator!=(const JsonProtocolSession& other) const;

	bool isValid() const;

	explicit operator bool() const;

private:
	JsonProtocolServer* _owner;
	JsonProtocolSessionID _sessionId;
};

namespace std
{
	template <>
	struct hash<JsonProtocolSession>
	{
		size_t operator()(const JsonProtocolSession& session) const noexcept
		{
			return std::hash<JsonProtocolSessionID>{}(session.getsessionId());
		}
	};
}

class JsonProtocolServer
{
	using CallBackRecvJsonMessage = std::function<Task<void>(JsonProtocolSession, json&)>;
	using CallBackRecvJsonRequest = std::function<Task<void>(JsonProtocolSession, json&, json&)>;
	using CallBackSessionEstablish = std::function<Task<void>(JsonProtocolSession)>;
	using CallBackCloseConnect = std::function<Task<void>(JsonProtocolSession)>;

public:
	JsonProtocolServer();
	~JsonProtocolServer();

	bool Start(const std::string& IP, int Port);

	bool SetCallBackRecvJsonMessage(const JsonProtocolSession& session, CallBackRecvJsonMessage&& callback);
	bool SetCallBackRecvJsonRequest(const JsonProtocolSession& session, CallBackRecvJsonRequest&& callback);
	bool SetCallBackCloseConnect(const JsonProtocolSession& session, CallBackCloseConnect&& callback);
	void SetCallbackSessionEstablish(CallBackSessionEstablish&& callback);

	bool AsyncSend(const JsonProtocolSession& sessionId, const json& js);
	Task<bool> AwaitSend(const JsonProtocolSession& sessionId, const json& req, json& rsp);

	bool ReleaseSession(const JsonProtocolSessionID& sessionId);
	bool ReleaseSession(const JsonProtocolSession& session);

private:
	Task<void> RecvMessage(ConID conId, Buffer* recv);
	Task<void> RequestMessage(ConID conId, Buffer* recv, Buffer* resp);
	Task<void> CloseConnect(ConID conId);
	Task<void> ConnectionEstablish(ConID conId);

	bool ParseJson(Buffer& buf, json& js);

	bool IsValidSession(const JsonProtocolSession& session);

	Task<bool> ProcessWaitCon(json& js_src, ConID conId, json& js_dest);

private:
	struct SessionHandle
	{
		ConID connectionId;

		uint64_t sessionEstablishTimeSecond = 0;
		uint64_t lastActiveTimeSecond = 0;

		CallBackRecvJsonMessage c_message;
		CallBackRecvJsonRequest c_request;
		CallBackCloseConnect c_closeconnect;
	};

private:
	CustomTcpSessionConnectManager _sessionmanager;

	SafeBiDirectionalMap<JsonProtocolSessionID, std::shared_ptr<SessionHandle>, CoroCriticalSectionLock> _SessionIdToSessionHandle;
	SafeBiDirectionalMap<ConID, JsonProtocolSessionID, CoroCriticalSectionLock> _ConIdToSessionId;

	SafeSet<ConID, CoroCriticalSectionLock> _waitConId; // 等待协商的

	CallBackSessionEstablish _CallBackSessionEstablish;
};