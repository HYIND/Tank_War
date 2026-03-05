#include "SessionManager/BaseSessionManager.h"
#include "tools.h"
#include "stdafx.h"

BaseSessionManager::BaseSessionManager(SessionType type)
    : type(type)
{
    listener = std::make_unique<NetWorkSessionListener>(type);
    listener->BindSessionEstablishCallBack(std::bind(&BaseSessionManager::SessionEstablish, this, std::placeholders::_1));
}

BaseSessionManager::~BaseSessionManager()
{
    auto guard = ConIdToBaseNetWork.MakeLockGuard();
    auto sessions = ConIdToBaseNetWork.GetAllRights();
    try
    {
        for (auto session : sessions)
        {
            session->Release();
            ConIdToBaseNetWork.EraseByRight(session);
            DeleteLater(session);
        }
        listener.reset();
    }
    catch (const std::exception &e)
    {
        std::cerr << "~BaseSessionManager exception:" << e.what() << '\n';
    }
}

bool BaseSessionManager::Start(const std::string &IP, int Port)
{
    ip = IP;
    port = Port;
    if (!listener->Listen(IP, port))
    {
        perror("listen error !");
        return false;
    }
    return true;
}

void BaseSessionManager::SetCallBackSessionEstablish(CallBackSessionEstablish &&callback)
{
    _CallBackSessionEstablish = callback;
}

void BaseSessionManager::SetCallBackRecvMessage(CallBackRecvMessage &&callback)
{
    _CallBackRecvMessage = callback;
}

void BaseSessionManager::SetCallBackCloseConnect(CallBackCloseConnect &&callback)
{
    _CallBackCloseConnect = callback;
}

bool BaseSessionManager::AsyncSend(const ConID& conid, const Buffer& buf)
{
    BaseNetWorkSession *session;
    if (!ConIdToBaseNetWork.FindByLeft(conid, session) || !session)
        return false;

    return session->AsyncSend(buf);
}

bool BaseSessionManager::ReleaseSession(const ConID &conid)
{
    BaseNetWorkSession *session;
    {
        auto guard = ConIdToBaseNetWork.MakeLockGuard();
        if (!ConIdToBaseNetWork.FindByLeft(conid, session) || !session)
            return false;
        ConIdToBaseNetWork.EraseByLeft(conid);
    }
    DeleteLater(session);
    return true;
}

Task<void> BaseSessionManager::SessionEstablish(BaseNetWorkSession *session)
{
    co_await session->BindSessionCloseCallBack(std::bind(&BaseSessionManager::CloseConnect, this, std::placeholders::_1));
    co_await session->BindRecvDataCallBack(std::bind(&BaseSessionManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));

    ConID ConId = Tool::GenerateSimpleUuid();
    ConIdToBaseNetWork.InsertOrUpdate(ConId, session);

    auto callback = _CallBackSessionEstablish;
    if (callback)
        co_await callback(ConId);
}

Task<void> BaseSessionManager::RecvMessage(BaseNetWorkSession *session, Buffer *recv)
{
    ConID conid;
    if (!ConIdToBaseNetWork.FindByRight(session, conid) || conid.empty())
        co_return;

    auto callback = _CallBackRecvMessage;
    if (callback)
        co_await callback(conid, recv);
}

Task<void> BaseSessionManager::CloseConnect(BaseNetWorkSession *session)
{
    ConID conid;
    {
        auto guard = ConIdToBaseNetWork.MakeLockGuard();
        if (!ConIdToBaseNetWork.FindByRight(session, conid) || conid.empty())
            co_return;
        ConIdToBaseNetWork.EraseByLeft(conid);
    }
    auto callback = _CallBackCloseConnect;
    if (callback)
        co_await callback(conid);
    DeleteLater(session);
}