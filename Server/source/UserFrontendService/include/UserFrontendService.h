#pragma once

#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"
#include "LoginSubService.h"
#include "LobbySubService.h"
#include "BaseService.h"

using namespace ServiceRegistryDataDef;

class UserFrontendService : public BaseService
{

public:
    UserFrontendService();
    ~UserFrontendService();
    bool Start(const std::string &IP, int Port);

    void SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();

private:
    std::shared_ptr<JsonCommunicateConnectManager> _connectmanager;
    std::unique_ptr<LoginSubService> _loginsubservice;
    std::unique_ptr<LobbySubService> _lobbysubservice;

    std::shared_ptr<ServiceInfo> _serviceinfo;
};