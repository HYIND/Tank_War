#include "LoginSubService.h"
#include "command.h"
#include "jwtHelper.h"
#include "tools.h"

using namespace LoginSubServiceCommand;

LoginSubService::LoginSubService()
{
}

LoginSubService::~LoginSubService()
{
}

Task<void> LoginSubService::OnSessionEstablish(JsonProtocolSession session)
{
    ConnectionEnter(session);
    co_return;
}

Task<void> LoginSubService::OnRecvMessage(JsonProtocolSession session, json &src)
{
    json dest;
    co_await ProcessMsg(session, src, dest);
    if (!dest.is_null())
        session.AsyncSendJson(dest);
    co_return;
}

Task<void> LoginSubService::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    co_await ProcessMsg(session, src, dest);
}

Task<void> LoginSubService::OnSessionClose(JsonProtocolSession session)
{
    co_return;
}

void LoginSubService::SetLobbySubService(std::shared_ptr<LobbySubService> service)
{
    _weaklobbyservice = std::weak_ptr<LobbySubService>(service);
}

void LoginSubService::SetServer(std::shared_ptr<JsonProtocolServer> m)
{
    _server = m;
    _server->SetCallbackSessionEstablish(std::bind(&LoginSubService::OnSessionEstablish, this, std::placeholders::_1));
}

void LoginSubService::SetGameStateStub(std::shared_ptr<JsonProtocolClient> stub)
{
    _gameStateStub = stub;
}

void LoginSubService::SetServiceInfo(std::shared_ptr<ServiceInfo> info)
{
    _serviceinfo = info;
}

bool LoginSubService::ConnectionEnter(JsonProtocolSession session)
{
    _server->SetCallBackRecvJsonMessage(session, std::bind(&LoginSubService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _server->SetCallBackRecvJsonRequest(session, std::bind(&LoginSubService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _server->SetCallBackCloseConnect(session, std::bind(&LoginSubService::OnSessionClose, this, std::placeholders::_1));

    return true;
}

Task<void> LoginSubService::ProcessMsg(JsonProtocolSession session, json& js_src, json& js_dest)
{
    if (!js_src.contains("command"))
        co_return;
    int command = js_src.at("command");

    if (command == LoginSubService_Login)
    {
        co_await ProcessLogin(session, js_src, js_dest);
    }
}

Task<void> LoginSubService::ProcessLogin(JsonProtocolSession session, json &js_src, json &js_dest)
{

    js_dest["command"] = LoginSubService_LoginRes;
    std::string name = js_src.value("name", "");
    if (name.empty())
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "用户名不能为空";
        co_return;
    }

    std::string uuid = Tool::GenerateSimpleUuid();
    auto u = std::make_shared<LobbyUser>();
    u->token = uuid;
    u->name = name;
    u->session = session;

    auto lobbyservice = _weaklobbyservice.lock();
    if (lobbyservice && lobbyservice->UserEnter(u))
    {
        js_dest["result"] = 1;
        js_dest["token"] = u->token;
        js_dest["jwt"] = GnenerateJwtStr(u->token);
        js_dest["name"] = u->name;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误";
        co_return;
    }
}
