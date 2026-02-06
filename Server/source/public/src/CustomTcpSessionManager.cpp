#include "SessionManager/CustomTcpSessionManager.h"

CustomTcpSessionConnectManager::CustomTcpSessionConnectManager()
    : BaseSessionManager(SessionType::CustomTCPSession)
{
}

void CustomTcpSessionConnectManager::SetCallBackRequestMessage(CallBackRequestMessage &&callback)
{
    _CallBackRequestMessage = callback;
}

bool CustomTcpSessionConnectManager::AwaitSend(const std::string &conid, const Buffer &buf, Buffer &response)
{
    BaseNetWorkSession *session;
    if (!ConIdToBaseNetWork.FindByLeft(conid, session) || !session)
        return false;

    return ((CustomTcpSession *)session)->AwaitSend(buf, response);
}

void CustomTcpSessionConnectManager::SessionEstablish(BaseNetWorkSession *session)
{
    CustomTcpSession *customtcpsession = (CustomTcpSession *)session;

    customtcpsession->BindRecvDataCallBack(std::bind(&BaseSessionManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    customtcpsession->BindSessionCloseCallBack(std::bind(&BaseSessionManager::CloseConnect, this, std::placeholders::_1));
    customtcpsession->BindRecvRequestCallBack(std::bind(&CustomTcpSessionConnectManager::RequestMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    std::string ConId = Tool::GenerateSimpleUuid();
    ConIdToBaseNetWork.InsertOrUpdate(ConId, session);

    if (_CallBackSessionEstablish)
        _CallBackSessionEstablish(ConId);
}

void CustomTcpSessionConnectManager::RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp)
{
    std::string conid;
    if (!ConIdToBaseNetWork.FindByRight(session, conid) || conid.empty())
        return;

    if (_CallBackRequestMessage)
        _CallBackRequestMessage(conid, recv, resp);
}
