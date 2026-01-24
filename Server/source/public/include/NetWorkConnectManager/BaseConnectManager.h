#pragma once

#include "stdafx.h"
#include "Net/Session/BaseNetWorkSession.h"

class BaseConnectManager
{
    using CallBackSessionEstablish = std::function<void(BaseNetWorkSession *)>;
    using CallBackRecvMessage = std::function<void(BaseNetWorkSession *, Buffer *)>;
    using CallBackRequestMessage = std::function<void(BaseNetWorkSession *, Buffer *, Buffer *)>;
    using CallBackCloseConnect = std::function<void(BaseNetWorkSession *)>;

public:
    BaseConnectManager(SessionType type);
    bool Start(const std::string &IP, int Port);

    void SetCallBackSessionEstablish(CallBackSessionEstablish &&callback);
    void SetCallBackRecvMessage(CallBackRecvMessage &&callback);
    void SetCallBackCloseConnect(CallBackCloseConnect &&callback);

public:
    virtual void SessionEstablish(BaseNetWorkSession *session);
    virtual void RecvMessage(BaseNetWorkSession *session, Buffer *recv);
    virtual void CloseConnect(BaseNetWorkSession *session);

protected:
    std::unique_ptr<NetWorkSessionListener> listener;
    SessionType type;

    SafeArray<BaseNetWorkSession *> sessions;
    std::string ip;
    int port;

    CallBackSessionEstablish _CallBackSessionEstablish;
    CallBackRecvMessage _CallBackRecvMessage;
    CallBackCloseConnect _CallBackCloseConnect;
};