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

void LoginSubService::OnSessionEstablish(JsonProtocolSession session)
{
    ConnectionEnter(session);
}

void LoginSubService::OnRecvMessage(JsonProtocolSession session, json &src)
{
    std::vector<json> dests;
    ProcessMsg(session, src, dests);
    if (!dests.empty())
    {
        for (auto &js : dests)
        {
            session.AsyncSendJson(js);
        }
    }
}

void LoginSubService::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    std::vector<json> dests;
    ProcessMsg(session, src, dests);
    if (!dests.empty())
        dest = dests[0];
}

void LoginSubService::OnSessionClose(JsonProtocolSession session)
{
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

void LoginSubService::ProcessMsg(JsonProtocolSession session, json &js_src, std::vector<json> &js_dests)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    if (command == LoginSubService_Login)
    {
        json js_dest;
        ProcessLogin(session, js_src, js_dest);
        js_dests.emplace_back(js_dest);
    }
}

void LoginSubService::ProcessLogin(JsonProtocolSession session, json &js_src, json &js_dest)
{

    js_dest["command"] = LoginSubService_LoginRes;
    std::string name = js_src.value("name", "");
    if (name.empty())
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "用户名不能为空";
        return;
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
        return;
    }
}
