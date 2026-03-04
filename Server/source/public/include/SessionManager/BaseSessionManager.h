#pragma once

#include "Net/Session/SessionListener.h"
#include "Net/Session/BaseNetWorkSession.h"
#include "BiDirectionalMap.h"
#include "stdafx.h"

using ConID = std::string;

class BaseSessionManager
{
    using CallBackSessionEstablish = std::function<Task<void>(ConID)>;
    using CallBackRecvMessage = std::function<Task<void>(ConID, Buffer *)>;
    using CallBackCloseConnect = std::function<Task<void>(ConID)>;

public:
    BaseSessionManager(SessionType type);
    ~BaseSessionManager();
    bool Start(const std::string &IP, int Port);

    void SetCallBackSessionEstablish(CallBackSessionEstablish &&callback);
    void SetCallBackRecvMessage(CallBackRecvMessage &&callback);
    void SetCallBackCloseConnect(CallBackCloseConnect &&callback);

    bool AsyncSend(const ConID& conid, const Buffer& buf);
    bool ReleaseSession(const ConID& conid);

public:
    virtual Task<void> SessionEstablish(BaseNetWorkSession *session);
    virtual Task<void> RecvMessage(BaseNetWorkSession *session, Buffer *recv);
    virtual Task<void> CloseConnect(BaseNetWorkSession *session);

protected:
    std::string ip;
    int port;
    std::unique_ptr<NetWorkSessionListener> listener;
    SessionType type;

    SafeBiDirectionalMap<ConID, BaseNetWorkSession *> ConIdToBaseNetWork;

    CallBackSessionEstablish _CallBackSessionEstablish;
    CallBackRecvMessage _CallBackRecvMessage;
    CallBackCloseConnect _CallBackCloseConnect;
};