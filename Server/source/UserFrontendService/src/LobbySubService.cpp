#include "LobbySubService.h"
#include "command.h"
#include "jwtHelper.h"

using namespace LoginSubServiceCommand;

LobbySubService::LobbySubService()
{
}

LobbySubService::~LobbySubService()
{
}

void LobbySubService::OnRecvMessage(BaseNetWorkSession *session, json &src)
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

void LobbySubService::OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest)
{
    std::vector<json> dests;
    ProcessMsg(session, src, dests);
    if (!dest.empty())
        dest = dest[0];
}

void LobbySubService::OnSessionClose(BaseNetWorkSession *session)
{
}

void LobbySubService::SetLoginSubService(std::shared_ptr<LoginSubService> service)
{
    _weakloginSubservice = std::weak_ptr<LoginSubService>(service);
}

void LobbySubService::SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m)
{
    _connectmanager = m;
}

bool LobbySubService::UserEnter(std::shared_ptr<LobbyUser> u)
{
    _allUsers.Insert(u->token, u);
    return true;
}

void LobbySubService::ProcessMsg(BaseNetWorkSession *session, json &js_src, std::vector<json> &js_dests)
{
    if (!js_src.contains("command"))
        return;

    if (!js_src.contains("jwt"))
        return;

    std::string token;
    std::shared_ptr<LobbyUser> user;
    {
        std::string jwtstr = js_src["jwt"];
        if (!VerfiyJwtToken(jwtstr, token))
            return;
        if (!_allUsers.Find(token, user) || !user)
            return;
        if (user->session != session)
            return;
    }

    int command = js_src.at("command");
    if (command == LobbySubService_Logout)
    {
        json js_dest;
        ProcessLogout(user, js_src, js_dest);
        js_dests.emplace_back(js_dest);
    }
}

void LobbySubService::ProcessLogout(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_LogoutRes;
    auto loginservice = _weakloginSubservice.lock();
    if (loginservice && loginservice->ConnectionEnter(user->session))
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误";
        return;
    }
}
