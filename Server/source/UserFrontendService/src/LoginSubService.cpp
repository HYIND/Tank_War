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

void LoginSubService::OnSessionEstablish(BaseNetWorkSession *session)
{
    ConnectionEnter(session);
}

void LoginSubService::OnRecvMessage(BaseNetWorkSession *session, json &src)
{
    std::vector<json> dests;
    ProcessMsg(session, src, dests);
    if (!dests.empty())
    {
        for (auto &js : dests)
        {
            const std::string js_str = js.dump();
            Buffer buf(js_str.c_str(), js_str.length());
            session->AsyncSend(js_str);
        }
    }
}

void LoginSubService::OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest)
{
    std::vector<json> dests;
    ProcessMsg(session, src, dests);
    if (!dest.empty())
        dest = dest[0];
}

void LoginSubService::OnSessionClose(BaseNetWorkSession *session)
{
}

void LoginSubService::SetLobbySubService(std::shared_ptr<LobbySubService> service)
{
    _weaklobbyservice = std::weak_ptr<LobbySubService>(service);
}

void LoginSubService::SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m)
{
    _connectmanager = m;
    _connectmanager->SetCallbackSessionEstablish(std::bind(&LoginSubService::OnSessionEstablish, this, std::placeholders::_1));
}

bool LoginSubService::ConnectionEnter(BaseNetWorkSession *session)
{
    _connectmanager->SetCallBackRecvJsonMessage(session, std::bind(&LoginSubService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _connectmanager->SetCallBackRecvJsonRequest(session, std::bind(&LoginSubService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _connectmanager->SetCallBackCloseConnect(session, std::bind(&LoginSubService::OnSessionClose, this, std::placeholders::_1));

    return true;
}

void LoginSubService::ProcessMsg(BaseNetWorkSession *session, json &js_src, std::vector<json> &js_dests)
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

void LoginSubService::ProcessLogin(BaseNetWorkSession *session, json &js_src, json &js_dest)
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
