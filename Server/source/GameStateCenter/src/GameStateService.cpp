#include "GameStateService.h"
#include "tools.h"

GameStateService::GameStateService()
{
    _serviceinfo = std::make_shared<ServiceInfo>(Tool::GenerateSimpleUuid(), ServiceType::GAMESTATE);
}

GameStateService::~GameStateService()
{
}

bool GameStateService::Start(const std::string &IP, int Port)
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

void GameStateService::SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m)
{
    assert(m);
    if (m != _connectmanager)
        _connectmanager = m;

    _connectmanager->SetCallbackSessionEstablish(std::bind(&GameStateService::OnSessionEstablish, this, std::placeholders::_1));
}

void GameStateService::SetGameStateManager(std::shared_ptr<GameStateManager> gsm)
{
    _GSM = gsm;
}

void GameStateService::OnSessionEstablish(BaseNetWorkSession *session)
{
    _connectmanager->SetCallBackRecvJsonMessage(session, std::bind(&GameStateService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _connectmanager->SetCallBackRecvJsonRequest(session, std::bind(&GameStateService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _connectmanager->SetCallBackCloseConnect(session, std::bind(&GameStateService::OnSessionClose, this, std::placeholders::_1));
}

void GameStateService::OnRecvMessage(BaseNetWorkSession *session, json &src)
{
    json dest;
    ProcessMsg(src, dest);
    if (!dest.is_null())
    {
        const std::string js_str = dest.dump();
        Buffer buf(js_str.c_str(), js_str.length());
        session->AsyncSend(js_str);
    }
}

void GameStateService::OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest)
{
    ProcessMsg(src, dest);
    if (!dest.is_null())
        dest = dest[0];
}

void GameStateService::OnSessionClose(BaseNetWorkSession *session)
{
}

std::vector<ServiceInfo> GameStateService::GetServiceInfo()
{
    return std::vector<ServiceInfo>{*_serviceinfo};
}

void GameStateService::ProcessMsg(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    // if (command == LoginSubService_Login)
    // {
    //     ProcessLogin(js_src, js_dest);
    // }
}
