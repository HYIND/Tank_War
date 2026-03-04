#pragma once

#include "Net/Session/CustomTcpSession.h"
#include "SessionManager/BaseSessionManager.h"
#include "stdafx.h"

class CustomTcpSessionConnectManager : public BaseSessionManager
{
    using CallBackRequestMessage = std::function<Task<void>(ConID, Buffer *, Buffer *)>;

public:
    CustomTcpSessionConnectManager();

    void SetCallBackRequestMessage(CallBackRequestMessage &&callback);

    Task<bool> AwaitSend(const std::string &conid, const Buffer &buf, Buffer &response);

public:
    virtual Task<void> SessionEstablish(BaseNetWorkSession *session);
    virtual Task<void> RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp);

protected:
    CallBackRequestMessage _CallBackRequestMessage;
};