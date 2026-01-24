#pragma once

#include "stdafx.h"
#include "LobbySubService.h"
#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"

class LobbySubService;
class LoginSubService
{
public:
    LoginSubService();
    ~LoginSubService();

public:
    void OnSessionEstablish(BaseNetWorkSession *session);
    void OnRecvMessage(BaseNetWorkSession *session, json &src);
    void OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest);
    void OnSessionClose(BaseNetWorkSession *session);

    void SetLobbySubService(std::shared_ptr<LobbySubService> service);
    void SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m);

public:
    bool ConnectionEnter(BaseNetWorkSession *session);
    
private:
    void ProcessMsg(BaseNetWorkSession *session, json &src, std::vector<json> &dest);
    void ProcessLogin(BaseNetWorkSession *session, json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonCommunicateConnectManager> _connectmanager;
    std::weak_ptr<LobbySubService> _weaklobbyservice;
};