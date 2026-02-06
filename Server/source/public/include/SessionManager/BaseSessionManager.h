#pragma once

#include "stdafx.h"
#include "Net/Session/SessionListener.h"
#include "Net/Session/BaseNetWorkSession.h"
#include "BiDirectionalMap.h"

class BaseSessionManager
{
    using CallBackSessionEstablish = std::function<void(std::string)>;
    using CallBackRecvMessage = std::function<void(std::string, Buffer *)>;
    using CallBackRequestMessage = std::function<void(std::string, Buffer *, Buffer *)>;
    using CallBackCloseConnect = std::function<void(std::string)>;

public:
    BaseSessionManager(SessionType type);
    ~BaseSessionManager();
    bool Start(const std::string &IP, int Port);

    void SetCallBackSessionEstablish(CallBackSessionEstablish &&callback);
    void SetCallBackRecvMessage(CallBackRecvMessage &&callback);
    void SetCallBackCloseConnect(CallBackCloseConnect &&callback);

    bool AsyncSend(const std::string &conid, const Buffer &buf);
    bool ReleaseSession(const std::string &conid);

public:
    virtual void SessionEstablish(BaseNetWorkSession *session);
    virtual void RecvMessage(BaseNetWorkSession *session, Buffer *recv);
    virtual void CloseConnect(BaseNetWorkSession *session);

protected:
    std::string ip;
    int port;
    std::unique_ptr<NetWorkSessionListener> listener;
    SessionType type;

    SafeBiDirectionalMap<std::string, BaseNetWorkSession *> ConIdToBaseNetWork;

    CallBackSessionEstablish _CallBackSessionEstablish;
    CallBackRecvMessage _CallBackRecvMessage;
    CallBackCloseConnect _CallBackCloseConnect;
};