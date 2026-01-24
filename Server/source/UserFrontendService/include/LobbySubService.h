#pragma once

#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"
#include "LobbyUserDef.h"
#include "LoginSubService.h"

class LoginSubService;
class LobbySubService
{
public:
    LobbySubService();
    ~LobbySubService();

public:
    void OnRecvMessage(BaseNetWorkSession *session, json &src);
    void OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest);
    void OnSessionClose(BaseNetWorkSession *session);

    void SetLoginSubService(std::shared_ptr<LoginSubService> service);
    void SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m);

public:
    bool UserEnter(std::shared_ptr<LobbyUser> u);

private:
    void ProcessMsg(BaseNetWorkSession *session, json &src, std::vector<json> &dest);
    void ProcessLogout(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonCommunicateConnectManager> _connectmanager;
    std::weak_ptr<LoginSubService> _weakloginSubservice;
    SafeMap<std::string, std::shared_ptr<LobbyUser>> _allUsers;
};