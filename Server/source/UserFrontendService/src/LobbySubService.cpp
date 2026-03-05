#include "LobbySubService.h"
#include "command.h"
#include "jwtHelper.h"
#include "GameStateDef/RoomDef.h"

using namespace LobbySubServiceCommand;
using namespace GameStateServiceCommand;

LobbySubService::LobbySubService()
{
}

LobbySubService::~LobbySubService()
{
}

Task<void> LobbySubService::OnRecvMessage(JsonProtocolSession session, json &src)
{
    json dest;
    co_await ProcessMsg(session, src, dest);
    if (!dest.is_null())
        session.AsyncSendJson(dest);
    co_return;
}

Task<void> LobbySubService::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    co_await ProcessMsg(session, src, dest);
}

Task<void> LobbySubService::OnSessionClose(JsonProtocolSession session)
{
    std::shared_ptr<LobbyUser> user;
    if (_sessionToUser.Find(session, user) && user)
    {
        json src, dest;
        co_await ProcessLogout(user, src, dest);
    }
    co_return;
}

void LobbySubService::SetLoginSubService(std::shared_ptr<LoginSubService> service)
{
    _weakloginSubservice = std::weak_ptr<LoginSubService>(service);
}

void LobbySubService::SetServer(std::shared_ptr<JsonProtocolServer> m)
{
    _server = m;
}

void LobbySubService::SetGameStateStub(std::shared_ptr<JsonProtocolClient> stub)
{
    _gameStateStub = stub;
}

void LobbySubService::SetServiceInfo(std::shared_ptr<ServiceInfo> info)
{
    _serviceinfo = info;
}

void LobbySubService::SetMessageDelegator(std::shared_ptr<MessageDelegator> delegator)
{
    _delegator = delegator;
}

Task<bool> LobbySubService::UserEnter(std::shared_ptr<LobbyUser> u)
{
    if (!co_await Stub_Login(u))
        co_return false;

    _tokenToUser.Insert(u->token, u);
    _sessionToUser.Insert(u->session, u);
    _server->SetCallBackRecvJsonMessage(u->session, std::bind(&LobbySubService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _server->SetCallBackRecvJsonRequest(u->session, std::bind(&LobbySubService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _server->SetCallBackCloseConnect(u->session, std::bind(&LobbySubService::OnSessionClose, this, std::placeholders::_1));
    co_return true;
}

Task<void> LobbySubService::OnStubRequest(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        co_return;

    int command = js_src.at("command");
    if (command == MessageDelegateCommand::MessageDelegate_UserMessageDelegate)
    {
        if (js_src.contains("delegate_token") && js_src.contains("content"))
        {
            js_dest["result"] = 1;
            std::string delegate_token = js_src.value("delegate_token", "");
            json content = js_src["content"];

            std::shared_ptr<LobbyUser> user;
            if (_tokenToUser.Find(delegate_token, user) && user)
                user->session.AsyncSendJson(content);
        }
        else
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "不完整的委托消息！";
        }
    }
}

Task<void> LobbySubService::ProcessMsg(JsonProtocolSession session, json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        co_return;

    if (!js_src.contains("jwt"))
        co_return;

    std::string token;
    std::shared_ptr<LobbyUser> user;
    {
        std::string jwtstr = js_src["jwt"];
        if (!VerfiyJwtToken(jwtstr, token))
            co_return;
        if (!_tokenToUser.Find(token, user) || !user)
            co_return;
        if (user->session != session)
            co_return;
    }

    int command = js_src.at("command");
    switch (command)
    {
    case LobbySubService_Logout:
        co_await ProcessLogout(user, js_src, js_dest);
        break;

    case LobbySubService_SendHallMsg:
        co_await ProcessHallMsg(user, js_src, js_dest);
        break;

    case LobbySubService_SendRoomMsg:
        co_await ProcessRoomMsg(user, js_src, js_dest);
        break;

    case LobbySubService_SendPrivateMsg:
        co_await ProcessPrivateMsg(user, js_src, js_dest);
        break;

    case LobbySubService_RequestLobbyUser:
        co_await ProcessRequestLobbyUser(user, js_src, js_dest);
        break;

    case LobbySubService_RequestLobbyRoom:
        co_await ProcessRequestLobbyRoom(user, js_src, js_dest);
        break;

    case LobbySubService_RequestRoomInfo:
        co_await ProcessRequestRoomInfo(user, js_src, js_dest);
        break;

    case LobbySubService_RequestCreateRoom:
        co_await ProcessRequestCreateRoom(user, js_src, js_dest);
        break;

    case LobbySubService_RequestJoinRoom:
        co_await ProcessRequestJoinRoom(user, js_src, js_dest);
        break;

    case LobbySubService_RequestLeaveRoom:
        co_await ProcessRequestLeaveRoom(user, js_src, js_dest);
        break;

    case LobbySubService_RequestChangeReadyStatus:
        co_await ProcessRequestChangeReadyStatus(user, js_src, js_dest);
        break;

    case LobbySubService_RequestStartGame:
        co_await ProcessRequestStartGame(user, js_src, js_dest);
        break;

    default:
        // ProcessUnknownCommand(user, js_src, js_dest);
        break;
    }
}

Task<void> LobbySubService::ProcessLogout(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_LogoutRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        success = co_await Stub_Logout(user);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        auto loginservice = _weakloginSubservice.lock();
        success = loginservice && loginservice->ConnectionEnter(user->session);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    _tokenToUser.Erase(user->token);
    _sessionToUser.Erase(user->session);
}

Task<void> LobbySubService::ProcessHallMsg(std::shared_ptr<LobbyUser> sender, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_SendHallMsgRes;

    bool success = true;
    std::string error_reason;

    std::string msg;
    if (success)
    {
        msg = js_src.value("msg", "");
        if (msg.empty())
        {
            success = false;
            error_reason = "不能发送空消息!";
        }
    }

    std::vector<LobbyUser> users;
    if (success)
    {
        success = co_await Stub_GetOnlineLobbyUser(users);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        json js_broadcast;
        js_broadcast["command"] = LobbySubService_HallMsgBroadcast;
        js_broadcast["srctoken"] = sender->token;
        js_broadcast["name"] = sender->name;
        js_broadcast["msg"] = msg;
        for (auto &user : users)
        {
            co_await DelegateDispatch(user.token, js_broadcast);
        }
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRoomMsg(std::shared_ptr<LobbyUser> sender, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_SendRoomMsgRes;

    bool success = true;
    std::string error_reason;

    std::string msg;
    if (success)
    {
        msg = js_src.value("msg", "");
        if (msg.empty())
        {
            success = false;
            error_reason = "不能发送空消息!";
        }
    }

    json js_roominfo;
    if (success)
    {
        success = co_await Stub_RoomInfo(sender, js_roominfo);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    GameStateDef::Room roominfo;
    if (success)
    {
        if (!js_roominfo.is_object())
        {
            success = false;
            error_reason = "服务器解析错误!";
        }
        else
            roominfo.UpdateFromJson(js_roominfo);
    }

    if (success)
    {
        json js_broadcast;
        js_broadcast["command"] = LobbySubService_RoomMsgBroadcast;
        js_broadcast["srctoken"] = sender->token;
        js_broadcast["name"] = sender->name;
        js_broadcast["msg"] = msg;

        co_await roominfo.members.AsyncEnsureCall(
            [&](std::map<std::string, std::shared_ptr<GameStateDef::RoomMemeber>> &map) -> Task<void>
            {
                std::vector<Task<bool>> tasks;

                for (auto &[token, memberinfo] : map)
                    tasks.push_back(std::move(DelegateDispatch(token, js_broadcast)));

                for (auto& task : tasks)
                    co_await task;
            });
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessPrivateMsg(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    co_return;
}

Task<void> LobbySubService::ProcessRequestLobbyUser(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{

    js_dest["command"] = LobbySubService_RequestLobbyUserRes;

    bool success = true;
    std::string error_reason;

    std::vector<LobbyUser> users;
    if (success)
    {
        success = co_await Stub_GetOnlineLobbyUser(users);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        json js_users = json::array();
        for (auto &user : users)
        {
            json js_user = {{"token", user.token},
                            {"name", user.name}};
            js_users.emplace_back(js_user);
        }
        js_dest["users"] = js_users;
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestLobbyRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{

    js_dest["command"] = LobbySubService_RequestLobbyRoomRes;

    bool success = true;
    std::string error_reason;

    json js_rooms;
    if (success)
    {
        success = co_await Stub_GetAllLobbyRoom(js_rooms);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
        js_dest["rooms"] = js_rooms;

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestRoomInfo(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_RequestRoomInfoRes;

    bool success = true;
    std::string error_reason;

    json roominfo;
    if (success)
    {
        success = co_await Stub_RoomInfo(user, roominfo);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["roominfo"] = roominfo;
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestCreateRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{

    js_dest["command"] = LobbySubService_RequestCreateRoomRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        success = co_await Stub_CreateRoom(user);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestJoinRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_RequestJoinRoomRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("roomid") || !js_src["roomid"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string roomid = js_src.value("roomid", "");
    if (success)
    {
        if (roomid.empty())
        {
            success = false;
            error_reason = "roomid不能为空";
        }
    }

    if (success)
    {
        success = co_await Stub_JoinRoom(user, roomid);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestLeaveRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_RequestLeaveRoomRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        success = co_await Stub_LeaveRoom(user);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestChangeReadyStatus(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_RequestChangeReadyStatusRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("status") || !js_src["status"].is_number_integer())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    int status = js_src.value("status", 0);

    if (success)
    {
        success = co_await Stub_ChangeReadyStatus(user, status);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<void> LobbySubService::ProcessRequestStartGame(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest)
{
    js_dest["command"] = LobbySubService_RequestStartGameRes;

    bool success = true;
    std::string error_reason;

    NetworkEndpoint endpoint;
    std::string gameid;
    if (success)
    {
        success = co_await Stub_StartGame(user, endpoint, gameid);
        if (!success)
            error_reason = "服务器内部错误!";
    }

    if (success)
    {
        js_dest["gameendpoint"] = endpoint.to_json();
        js_dest["gameid"] = gameid;
    }

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
    co_return;
}

Task<bool> LobbySubService::Stub_Login(std::shared_ptr<LobbyUser> user)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_UserLogin;
    js_req["token"] = user->token;
    js_req["name"] = user->name;
    js_req["serviceid"] = _serviceinfo->service_id;
    js_req["servicetype"] = (int)_serviceinfo->service_type;

    if (!co_await Stub_Request(js_req, js_resp))
        co_return false;

    co_return true;
}

Task<bool> LobbySubService::Stub_Logout(std::shared_ptr<LobbyUser> user)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_UserLogoutService;
    js_req["token"] = user->token;
    js_req["name"] = user->name;
    js_req["serviceid"] = _serviceinfo->service_id;
    js_req["servicetype"] = (int)_serviceinfo->service_type;

    if (!co_await Stub_Request(js_req, js_resp))
        co_return false;

    co_return true;
}

Task<bool> LobbySubService::Stub_GetOnlineLobbyUser(std::vector<LobbyUser> &users)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_GetOnlineLobbyUser;

    if (!co_await Stub_Request(js_req, js_resp))
        co_return false;

    json js_users = js_resp["users"];
    if (!js_users.is_array())
    {
        std::cout << "GameStateService_GetOnlineLobbyUser Request fail with null array!\n";
        co_return false;
    }

    for (int i = 0; i < js_users.size(); i++)
    {
        json js_user = js_users.at(i);
        if (js_user.is_object() && js_user["token"].is_string() && js_user["name"].is_string())
        {
            LobbyUser u;
            u.token = js_user["token"];
            u.name = js_user["name"];
            if (u.token.empty() || u.name.empty())
                continue;
            users.emplace_back(std::move(u));
        }
    }

    co_return true;
}

Task<bool> LobbySubService::Stub_RoomInfo(std::shared_ptr<LobbyUser> user, json &js_roominfo)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_RoomInfo;
    js_req["token"] = user->token;

    if (!co_await Stub_Request(js_req, js_resp))
        co_return false;

    if (!js_resp["roominfo"].is_object())
    {
        std::cout << "GameStateService_GetOnlineLobbyUser Request fail with null array!\n";
        co_return false;
    }

    js_roominfo = js_resp["roominfo"];
    co_return true;
}

Task<bool> LobbySubService::Stub_CreateRoom(std::shared_ptr<LobbyUser> user)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_CreateRoom;
    js_req["token"] = user->token;

    co_return co_await Stub_Request(js_req, js_resp);
}

Task<bool> LobbySubService::Stub_JoinRoom(std::shared_ptr<LobbyUser> user, const std::string &roomid)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_JoinRoom;
    js_req["token"] = user->token;
    js_req["roomid"] = roomid;

    co_return co_await Stub_Request(js_req, js_resp);
}

Task<bool> LobbySubService::Stub_LeaveRoom(std::shared_ptr<LobbyUser> user)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_LeaveRoom;
    js_req["token"] = user->token;

    co_return co_await Stub_Request(js_req, js_resp);
}

Task<bool> LobbySubService::Stub_ChangeReadyStatus(std::shared_ptr<LobbyUser> user, int status)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_ChangeReadyStatus;
    js_req["token"] = user->token;
    js_req["status"] = status;

    co_return co_await Stub_Request(js_req, js_resp);
}

Task<bool> LobbySubService::Stub_StartGame(std::shared_ptr<LobbyUser> user, NetworkEndpoint &endpoint, std::string &gameid)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_StartGame;
    js_req["token"] = user->token;

    if (!co_await Stub_Request(js_req, js_resp))
        co_return false;

    if (!js_resp.contains("gameendpoint") || !js_resp["gameendpoint"].is_object())
        co_return false;

    endpoint.update_from_json(js_resp["gameendpoint"]);

    if (!js_resp.contains("gameid") ||
        !js_resp["gameid"].is_string())
        co_return false;

    gameid = js_resp.value("gameid", "");

    co_return true;
}

Task<bool> LobbySubService::Stub_GetAllLobbyRoom(json &js_rooms)
{
    json js_req, js_resp;
    js_req["command"] = GameStateService_GetAllLobbyRoom;

    if (!co_await Stub_Request(js_req, js_resp))
        co_return false;

    if (!js_resp["rooms"].is_array())
    {
        std::cout << "GameStateService_GetOnlineLobbyUser Request fail with null array!\n";
        co_return false;
    }

    js_rooms = js_resp["rooms"];

    co_return true;
}

Task<bool> LobbySubService::Stub_Request(const json &js_request, json &response)
{
    if (!co_await _gameStateStub->Request(js_request, response))
        co_return false;

    if (!response.contains("result") || !response["result"].is_number_integer())
        co_return false;

    int result = response["result"];
    if (result < 0)
    {
        std::string reason = response.value("reason", "");
        std::cout << "Stub Request fail! command =" << js_request.value("command", 0) << " reason:" << reason << '\n';
        co_return false;
    }
    co_return true;
}

Task<bool> LobbySubService::DelegateDispatch(const std::string &user_token, const json &js)
{
    if (!_delegator)
        co_return false;

    if (_delegator->SkipLocalService())
    {
        std::shared_ptr<LobbyUser> local_user;
        if (_tokenToUser.Find(user_token, local_user) && local_user)
            local_user->session.AsyncSendJson(js);
    }
    co_return co_await _delegator->DelegateMessage(user_token, js);
}
