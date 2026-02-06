#pragma once

#include "stdafx.h"
#include "SessionManager/BaseSessionManager.h"
#include "Net/Session/CustomTcpSession.h"

class CustomTcpSessionConnectManager : public BaseSessionManager
{
    using CallBackRequestMessage = std::function<void(std::string, Buffer *, Buffer *)>;

public:
    CustomTcpSessionConnectManager();

    void SetCallBackRequestMessage(CallBackRequestMessage &&callback);

    bool AwaitSend(const std::string &conid, const Buffer &buf, Buffer &response);

public:
    virtual void SessionEstablish(BaseNetWorkSession *session);
    virtual void RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp);

protected:
    CallBackRequestMessage _CallBackRequestMessage;
};