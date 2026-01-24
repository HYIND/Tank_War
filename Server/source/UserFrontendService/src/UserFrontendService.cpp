#include "UserFrontendService.h"
#include "tools.h"

UserFrontendService::UserFrontendService()
{
    _serviceinfo = std::make_shared<ServiceInfo>(Tool::GenerateSimpleUuid(), ServiceType::LOBBY);
    auto lobbydata = _serviceinfo->lobby_metadata();
    lobbydata->current_players = 0;
    lobbydata->max_players = 1000;

    _loginsubservice = std::make_unique<LoginSubService>();
    _lobbysubservice = std::make_unique<LobbySubService>();
}

UserFrontendService::~UserFrontendService()
{
}

bool UserFrontendService::Start(const std::string &IP, int Port)
{
    if (!_connectmanager)
    {
        _connectmanager = std::make_shared<JsonCommunicateConnectManager>();
        SetConnectManager(_connectmanager);
    }
    bool result = _connectmanager->Start(IP, Port);
    if (result)
        _serviceinfo->set_endpoint(IP, Port);
    else
        _serviceinfo->set_endpoint("", 0);
    return result;
}

void UserFrontendService::SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m)
{
    assert(m);
    if (m != _connectmanager)
        _connectmanager = m;
    _loginsubservice->SetConnectManager(_connectmanager);
    _lobbysubservice->SetConnectManager(_connectmanager);
}

std::vector<ServiceRegistryDataDef::ServiceInfo> UserFrontendService::GetServiceInfo()
{
    return std::vector<ServiceRegistryDataDef::ServiceInfo>{*_serviceinfo};
}
