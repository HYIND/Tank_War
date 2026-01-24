#include "GameService.h"

#include "tools.h"

GameService::GameService()
{
    _serviceinfo = std::make_shared<ServiceInfo>(Tool::GenerateSimpleUuid(), ServiceType::GAME);
}

GameService::~GameService()
{
}

bool GameService::Start(const std::string &IP, int Port)
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

void GameService::SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m)
{
    assert(m);
    if (m != _connectmanager)
        _connectmanager = m;

    _connectmanager->SetCallbackSessionEstablish(std::bind(&GameService::OnSessionEstablish, this, std::placeholders::_1));
}

void GameService::OnSessionEstablish(BaseNetWorkSession *session)
{
    _connectmanager->SetCallBackRecvJsonMessage(session, std::bind(&GameService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _connectmanager->SetCallBackRecvJsonRequest(session, std::bind(&GameService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _connectmanager->SetCallBackCloseConnect(session, std::bind(&GameService::OnSessionClose, this, std::placeholders::_1));
}

void GameService::OnRecvMessage(BaseNetWorkSession *session, json &src)
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

void GameService::OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest)
{
    ProcessMsg(src, dest);
    if (!dest.is_null())
        dest = dest[0];
}

void GameService::OnSessionClose(BaseNetWorkSession *session)
{
}

std::vector<ServiceInfo> GameService::GetServiceInfo()
{
    return std::vector<ServiceInfo>{*_serviceinfo};
}

void GameService::ProcessMsg(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    // if (command == LoginSubService_Login)
    // {
    //     ProcessLogin(js_src, js_dest);
    // }
}
