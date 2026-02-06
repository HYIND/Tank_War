#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "LoginSubService.h"
#include "LobbySubService.h"
#include "BaseService.h"

using namespace ServiceRegistryDataDef;

class LoginSubService;
class LobbySubService;
class UserFrontendService : public BaseService, public std::enable_shared_from_this<UserFrontendService>
{

public:
    UserFrontendService();
    ~UserFrontendService();

    void SetGameStateEndPoint(const std::string &IP, int Port);

    virtual bool Start();

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();
    virtual void OnStubRequest(json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonProtocolClient> _gameStateStub;
    std::string _gameStateStubIP;
    int _gameStateStubPort;

    std::shared_ptr<LoginSubService> _loginsubservice;
    std::shared_ptr<LobbySubService> _lobbysubservice;

    std::shared_ptr<ServiceInfo> _serviceinfo;

    std::shared_ptr<MessageDelegator> _delegator;
};