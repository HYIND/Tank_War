#include "SessionManager/CustomTcpSessionManager.h"

CustomTcpSessionConnectManager::CustomTcpSessionConnectManager()
    : BaseSessionManager(SessionType::CustomTCPSession)
{
}

void CustomTcpSessionConnectManager::SetCallBackRequestMessage(CallBackRequestMessage &&callback)
{
    _CallBackRequestMessage = callback;
}

Task<bool> CustomTcpSessionConnectManager::AwaitSend(const ConID& conid, const Buffer& buf, Buffer& response)
{
    BaseNetWorkSession *session;
    if (!ConIdToBaseNetWork.FindByLeft(conid, session) || !session)
        co_return false;

    auto result = co_await ((CustomTcpSession*)session)->AwaitSend(buf);
    if (result->code != AwaitErrorCode::Success)
        co_return false;

    response.QuoteFromBuf(result->response);
    co_return true;
}

Task<void> CustomTcpSessionConnectManager::SessionEstablish(BaseNetWorkSession *session)
{
    CustomTcpSession *customtcpsession = (CustomTcpSession *)session;

    customtcpsession->BindRecvDataCallBack(std::bind(&BaseSessionManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    customtcpsession->BindSessionCloseCallBack(std::bind(&BaseSessionManager::CloseConnect, this, std::placeholders::_1));
    customtcpsession->BindRecvRequestCallBack(std::bind(&CustomTcpSessionConnectManager::RequestMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    ConID ConId = Tool::GenerateSimpleUuid();
    ConIdToBaseNetWork.InsertOrUpdate(ConId, session);

    auto callback = _CallBackSessionEstablish;
    if (callback)
        co_await callback(ConId);
}

Task<void> CustomTcpSessionConnectManager::RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp)
{
    ConID conid;
    if (!ConIdToBaseNetWork.FindByRight(session, conid) || conid.empty())
        co_return;

    auto callback = _CallBackRequestMessage;
    if (callback)
        co_await _CallBackRequestMessage(conid, recv, resp);
}
