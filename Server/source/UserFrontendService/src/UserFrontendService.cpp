#include "UserFrontendService.h"
#include "tools.h"
#include "command.h"
#include "EndPointConfig.h"

UserFrontendService::UserFrontendService()
{
    _gameStateStub = std::make_shared<JsonProtocolClient>();
    _serviceinfo = std::make_shared<ServiceInfo>(Tool::GenerateSimpleUuid(), ServiceType::LOBBY);
    _delegator = std::make_shared<MessageDelegator>();
    _delegator->SetGameStateServiceEndpoint(GameStatesServiceStubIP, GameStatesServiceStubPort);
    _delegator->SetServiceDiscoveryEndpoint(ServiceDiscoveryIP, ServiceDiscoveryPort);

    auto lobbydata = _serviceinfo->lobby_metadata();
    lobbydata->current_players = 0;
    lobbydata->max_players = 1000;

    _loginsubservice = std::make_shared<LoginSubService>();
    _lobbysubservice = std::make_shared<LobbySubService>();

    _loginsubservice->SetServer(_service_server);
    _lobbysubservice->SetServer(_service_server);

    _loginsubservice->SetLobbySubService(_lobbysubservice);
    _lobbysubservice->SetLoginSubService(_loginsubservice);

    _loginsubservice->SetGameStateStub(_gameStateStub);
    _lobbysubservice->SetGameStateStub(_gameStateStub);

    _loginsubservice->SetServiceInfo(_serviceinfo);
    _lobbysubservice->SetServiceInfo(_serviceinfo);

    _lobbysubservice->SetMessageDelegator(_delegator);

    _delegator->SetSkipLocalService(true);

    SetServiceEnable(true);
    SetStubEnable(true);
}

UserFrontendService::~UserFrontendService()
{
}

void UserFrontendService::SetGameStateEndPoint(const std::string &IP, int Port)
{
    _gameStateStubIP = IP;
    _gameStateStubPort = Port;
}

bool UserFrontendService::Start()
{
    bool result = _gameStateStub->Connect(_gameStateStubIP, _gameStateStubPort) && BaseService::Start();
    if (result)
    {
        _delegator->AddLocalService(shared_from_this(), _serviceinfo->service_id, _serviceinfo->service_type);
        if (ServiceEnable())
            _serviceinfo->set_endpoint(_serviceIP, _servicePort);
        if (StubEnable())
            _serviceinfo->set_stub_endpoint(_stubIP, _stubPort);
    }
    else
    {
        _serviceinfo->set_endpoint("", 0);
        _serviceinfo->set_stub_endpoint("", 0);
    }
    return result;
}

void UserFrontendService::OnStubRequest(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    _lobbysubservice->OnStubRequest(js_src, js_dest);
}

std::vector<ServiceRegistryDataDef::ServiceInfo> UserFrontendService::GetServiceInfo()
{
    return std::vector<ServiceRegistryDataDef::ServiceInfo>{*_serviceinfo};
}
