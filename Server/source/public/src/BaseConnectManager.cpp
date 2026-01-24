#include "stdafx.h"
#include "NetWorkConnectManager/BaseConnectManager.h"

BaseConnectManager::BaseConnectManager(SessionType type)
    : type(type)
{
    listener = std::make_unique<NetWorkSessionListener>(type);
    listener->BindSessionEstablishCallBack(std::bind(&BaseConnectManager::SessionEstablish, this, std::placeholders::_1));
}

bool BaseConnectManager::Start(const std::string &IP, int Port)
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

void BaseConnectManager::SetCallBackSessionEstablish(CallBackSessionEstablish &&callback)
{
    _CallBackSessionEstablish = callback;
}

void BaseConnectManager::SetCallBackRecvMessage(CallBackRecvMessage &&callback)
{
    _CallBackRecvMessage = callback;
}

void BaseConnectManager::SetCallBackCloseConnect(CallBackCloseConnect &&callback)
{
    _CallBackCloseConnect = callback;
}

void BaseConnectManager::SessionEstablish(BaseNetWorkSession *session)
{
    session->BindRecvDataCallBack(std::bind(&BaseConnectManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    session->BindSessionCloseCallBack(std::bind(&BaseConnectManager::CloseConnect, this, std::placeholders::_1));
    sessions.emplace(session);

    if (_CallBackSessionEstablish)
        _CallBackSessionEstablish(session);
}

void BaseConnectManager::RecvMessage(BaseNetWorkSession *session, Buffer *recv)
{
    if (_CallBackRecvMessage)
        _CallBackRecvMessage(session, recv);
}

void BaseConnectManager::CloseConnect(BaseNetWorkSession *session)
{
    sessions.EnsureCall(
        [&](std::vector<BaseNetWorkSession *> &array) -> void
        {
            for (auto it = array.begin(); it != array.end(); it++)
            {
                if ((*it) == session)
                {
                    array.erase(it);
                    return;
                }
            }
        }

    );

    if (_CallBackCloseConnect)
        _CallBackCloseConnect(session);

    DeleteLater(session);
}