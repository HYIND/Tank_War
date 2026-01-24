#pragma once

#include "stdafx.h"
#include "NetWorkConnectManager/BaseConnectManager.h"

class CustomTcpSessionConnectManager : public BaseConnectManager
{
    using CallBackRequestMessage = std::function<void(BaseNetWorkSession *, Buffer *, Buffer *)>;

public:
    CustomTcpSessionConnectManager();

    void SetCallBackRequestMessage(CallBackRequestMessage &&callback);

public:
    virtual void SessionEstablish(BaseNetWorkSession *session);
    virtual void RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp);

protected:
    CallBackRequestMessage _CallBackRequestMessage;
};