#pragma once

#include "SessionManager/CustomTcpSessionManager.h"
#include "JsonProtocolSession.h"
#include "BiDirectionalMap.h"
#include "SafeStl.h"
#include "stdafx.h"

class JsonProtocolSession;

class JsonProtocolServer
{
    using CallBackRecvJsonMessage = std::function<void(JsonProtocolSession, json &)>;
    using CallBackRecvJsonRequest = std::function<void(JsonProtocolSession, json &, json &)>;
    using CallBackSessionEstablish = std::function<void(JsonProtocolSession)>;
    using CallBackCloseConnect = std::function<void(JsonProtocolSession)>;

public:
    JsonProtocolServer();
    ~JsonProtocolServer();

    bool Start(const std::string &IP, int Port);

    bool SetCallBackRecvJsonMessage(const JsonProtocolSession& session, CallBackRecvJsonMessage &&callback);
    bool SetCallBackRecvJsonRequest(const JsonProtocolSession& session, CallBackRecvJsonRequest &&callback);
    bool SetCallBackCloseConnect(const JsonProtocolSession& session, CallBackCloseConnect &&callback);
    void SetCallbackSessionEstablish(CallBackSessionEstablish &&callback);

    bool AsyncSend(const JsonProtocolSession &sessionId, const json &js);
    bool AwaitSend(const JsonProtocolSession &sessionId, const json &req, json &rsp);

    bool ReleaseSession(const std::string &sessionId);
    bool ReleaseSession(const JsonProtocolSession &session);

private:
    void RecvMessage(std::string conId, Buffer *recv);
    void RequestMessage(std::string conId, Buffer *recv, Buffer *resp);
    void CloseConnect(std::string conId);
    void ConnectionEstablish(std::string conId);

    bool ParseJson(Buffer &buf, json &js);

    bool IsValidSession(const JsonProtocolSession &session);

private:
    struct SessionHandle
    {
        std::string connectionId;

        uint64_t sessionEstablishTimeSecond = 0;
        uint64_t lastActiveTimeSecond = 0;

        CallBackRecvJsonMessage c_message;
        CallBackRecvJsonRequest c_request;
        CallBackCloseConnect c_closeconnect;
    };

private:
    CustomTcpSessionConnectManager _sessionmanager;

    SafeBiDirectionalMap<std::string, std::shared_ptr<SessionHandle>> _SessionIdToSessionHandle;
    SafeBiDirectionalMap<std::string, std::string> _ConIdToSessionId;

    SafeSet<std::string> _waitConId; // 等待协商的

    CallBackSessionEstablish _CallBackSessionEstablish;
};