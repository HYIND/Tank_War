#pragma once

#include "stdafx.h"
#include "Net/Session/CustomTcpSession.h"

// 基于CustomTcpSession封装的Json通信客户端
class JsonProtocolClient
{
	using CallBackJsonMessage = std::function<void(JsonProtocolClient*, json&)>;
	using CallBackJsonRequest = std::function<void(JsonProtocolClient*, json&, json&)>;
	using CallBackCloseClient = std::function<void(JsonProtocolClient*)>;

public:
	JsonProtocolClient();
	~JsonProtocolClient();

	bool Release();

	bool Connect(const std::string& IP, uint16_t Port);

	bool Send(const json& js);
	bool Request(const json& js_request, json& response);

	void BindCallBackJsonMessage(CallBackJsonMessage&& callback);
	void BindCallBackJsonRequest(CallBackJsonRequest&& callback);
	void BindCallBackCloseClient(CallBackCloseClient&& callback);

public:
	void RecvData(BaseNetWorkSession* session, Buffer* recv);
	void RecvRequest(BaseNetWorkSession* session, Buffer* recv, Buffer* resp);
	void SessionClose(BaseNetWorkSession* session);

private:
	bool RequestSessionId();

private:
	std::string _sessionId;
	std::shared_ptr<CustomTcpSession> _session;
	CallBackJsonMessage _callbackjsonmessage;
	CallBackJsonRequest _callbackjsonrequest;
	CallBackCloseClient _callbackcloseclient;
};