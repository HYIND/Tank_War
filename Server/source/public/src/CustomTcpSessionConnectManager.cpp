#include "NetWorkConnectManager/CustomTcpSessionConnectManager.h"

CustomTcpSessionConnectManager::CustomTcpSessionConnectManager()
    : BaseConnectManager(SessionType::CustomTCPSession)
{
}

void CustomTcpSessionConnectManager::SetCallBackRequestMessage(CallBackRequestMessage &&callback)
{
    _CallBackRequestMessage = callback;
}

void CustomTcpSessionConnectManager::SessionEstablish(BaseNetWorkSession *session)
{
    CustomTcpSession *customtcpsession = (CustomTcpSession *)session;

    customtcpsession->BindRecvDataCallBack(std::bind(&BaseConnectManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    customtcpsession->BindSessionCloseCallBack(std::bind(&BaseConnectManager::CloseConnect, this, std::placeholders::_1));
    customtcpsession->BindRecvRequestCallBack(std::bind(&CustomTcpSessionConnectManager::RequestMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    sessions.emplace(session);

    if (_CallBackSessionEstablish)
        _CallBackSessionEstablish(session);
}

void CustomTcpSessionConnectManager::RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp)
{
    if (_CallBackRequestMessage)
        _CallBackRequestMessage(session, recv, resp);
}
