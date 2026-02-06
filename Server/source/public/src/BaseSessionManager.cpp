#include "stdafx.h"
#include "SessionManager/BaseSessionManager.h"
#include "tools.h"

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
            CloseConnect(session);
        }
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

bool BaseSessionManager::AsyncSend(const std::string &conid, const Buffer &buf)
{
    BaseNetWorkSession *session;
    if (!ConIdToBaseNetWork.FindByLeft(conid, session) || !session)
        return false;

    return session->AsyncSend(buf);
}

bool BaseSessionManager::ReleaseSession(const std::string &conid)
{
    BaseNetWorkSession *session;
    if (!ConIdToBaseNetWork.FindByLeft(conid, session) || !session)
        return false;

    {
        auto guard = ConIdToBaseNetWork.MakeLockGuard();
        ConIdToBaseNetWork.EraseByLeft(conid);
    }

    session->Release();

    DeleteLater(session);

    return true;
}

void BaseSessionManager::SessionEstablish(BaseNetWorkSession *session)
{
    session->BindRecvDataCallBack(std::bind(&BaseSessionManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    session->BindSessionCloseCallBack(std::bind(&BaseSessionManager::CloseConnect, this, std::placeholders::_1));

    std::string ConId = Tool::GenerateSimpleUuid();
    ConIdToBaseNetWork.InsertOrUpdate(ConId, session);

    if (_CallBackSessionEstablish)
        _CallBackSessionEstablish(ConId);
}

void BaseSessionManager::RecvMessage(BaseNetWorkSession *session, Buffer *recv)
{
    std::string conid;
    if (!ConIdToBaseNetWork.FindByRight(session, conid) || conid.empty())
        return;

    if (_CallBackRecvMessage)
        _CallBackRecvMessage(conid, recv);
}

void BaseSessionManager::CloseConnect(BaseNetWorkSession *session)
{
    std::string conid;
    if (!ConIdToBaseNetWork.FindByRight(session, conid) || conid.empty())
        return;

    {
        auto guard = ConIdToBaseNetWork.MakeLockGuard();
        ConIdToBaseNetWork.EraseByLeft(conid);
    }

    if (_CallBackCloseConnect)
        _CallBackCloseConnect(conid);

    DeleteLater(session);
}