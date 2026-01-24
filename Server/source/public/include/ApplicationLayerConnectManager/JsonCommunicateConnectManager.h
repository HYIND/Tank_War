#pragma once

#include "stdafx.h"
#include "NetWorkConnectManager/CustomTcpSessionConnectManager.h"

class JsonCommunicateConnectManager
{
    using CallBackRecvJsonMessage = std::function<void(BaseNetWorkSession *, json &)>;
    using CallBackRecvJsonRequest = std::function<void(BaseNetWorkSession *, json &, json &)>;
    using CallBackSessionEstablish = std::function<void(BaseNetWorkSession *)>;
    using CallBackCloseConnect = std::function<void(BaseNetWorkSession *)>;

public:
    JsonCommunicateConnectManager();

    bool Start(const std::string &IP, int Port);
    void SetCallBackRecvJsonMessage(BaseNetWorkSession *session, CallBackRecvJsonMessage &&callback);
    void SetCallBackRecvJsonRequest(BaseNetWorkSession *session, CallBackRecvJsonRequest &&callback);
    void SetCallBackCloseConnect(BaseNetWorkSession *session, CallBackCloseConnect &&callback);
    void SetCallbackSessionEstablish(CallBackSessionEstablish &&callback);

private:
    void RecvMessage(BaseNetWorkSession *session, Buffer *recv);
    void RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp);
    void CloseConnect(BaseNetWorkSession *session);
    void SessionEstablish(BaseNetWorkSession *session);

    bool ParseJson(Buffer &buf, json &js);

private:
    CustomTcpSessionConnectManager _sessionmanager;
    SafeMap<BaseNetWorkSession *, CallBackRecvJsonMessage> _CallBackRecvJsonMessage_Map;
    SafeMap<BaseNetWorkSession *, CallBackRecvJsonRequest> _CallBackRecvJsonRequest_Map;
    SafeMap<BaseNetWorkSession *, CallBackCloseConnect> _CallBackCloseConnect_Map;
    CallBackSessionEstablish _CallBackSessionEstablish;
};