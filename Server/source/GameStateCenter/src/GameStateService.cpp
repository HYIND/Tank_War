#include "GameStateService.h"
#include "tools.h"
#include "command.h"
#include "EndPointConfig.h"

using namespace GameStateServiceCommand;

GameStateService::GameStateService()
{
    _serviceinfo = std::make_shared<ServiceInfo>(Tool::GenerateSimpleUuid(), ServiceType::GAMESTATE);
    _service_server->SetCallbackSessionEstablish(std::bind(&GameStateService::OnSessionEstablish, this, std::placeholders::_1));
    SetServiceEnable(false);
    SetStubEnable(true);

    _delegator.SetGameStateServiceEndpoint(GameStatesServiceStubIP, GameStatesServiceStubPort);
    _delegator.SetServiceDiscoveryEndpoint(ServiceDiscoveryIP, ServiceDiscoveryPort);
    _delegator.SetSkipLocalService(false);

    _servcieDiscoverClient.SetEndpoint(ServiceDiscoveryIP, ServiceDiscoveryPort);
}

GameStateService::~GameStateService()
{
}

bool GameStateService::Start()
{
    bool result = BaseService::Start();
    if (result)
    {
        _delegator.AddLocalService(shared_from_this(), _serviceinfo->service_id, _serviceinfo->service_type);
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

void GameStateService::SetGameStateManager(std::shared_ptr<GameStateManager> gsm)
{
    _GSM_Weak = gsm;
}

void GameStateService::OnSessionEstablish(JsonProtocolSession session)
{
    _service_server->SetCallBackRecvJsonMessage(session, std::bind(&GameStateService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _service_server->SetCallBackRecvJsonRequest(session, std::bind(&GameStateService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _service_server->SetCallBackCloseConnect(session, std::bind(&GameStateService::OnSessionClose, this, std::placeholders::_1));
}

void GameStateService::OnRecvMessage(JsonProtocolSession session, json &src)
{
    json dest;
    ProcessMsg(src, dest);
    if (!dest.is_null())
        session.AsyncSendJson(dest);
}

void GameStateService::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    ProcessMsg(src, dest);
}

void GameStateService::OnSessionClose(JsonProtocolSession session)
{
}

std::vector<ServiceInfo> GameStateService::GetServiceInfo()
{
    return std::vector<ServiceInfo>{*_serviceinfo};
}

void GameStateService::OnStubRequest(json &js_src, json &js_dest)
{
    ProcessMsg(js_src, js_dest);
}

void GameStateService::ProcessMsg(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    switch (command)
    {
    case GameStateService_FindUserEndPoint:
        HandleFindUserEndPoint(js_src, js_dest);
        break;

    case GameStateService_UserLogin:
        HandleUserLogin(js_src, js_dest);
        break;

    case GameStateService_UserLogoutService:
        HandleUserLogoutService(js_src, js_dest);
        break;

    case GameStateService_GetOnlineLobbyUser:
        HandleGetOnlineLobbyUser(js_src, js_dest);
        break;

    case GameStateService_GetAllLobbyRoom:
        HandleGetAllLobbyRoom(js_src, js_dest);
        break;

    case GameStateService_CreateRoom:
        HandleCreateRoom(js_src, js_dest);
        break;

    case GameStateService_JoinRoom:
        HandleJoinRoom(js_src, js_dest);
        break;

    case GameStateService_LeaveRoom:
        HandleLeaveRoom(js_src, js_dest);
        break;

    case GameStateService_ChangeReadyStatus:
        HandleChangeReadyStatus(js_src, js_dest);
        break;

    case GameStateService_RoomInfo:
        HandleGetRoomInfo(js_src, js_dest);
        break;

    case GameStateService_StartGame:
        HandleStartGame(js_src, js_dest);
        break;

    case GameStateService_PlayerLeaveGame:
        HanlePlayerLeaveGame(js_src, js_dest);
        break;

    case GameStateService_GameEnd:
        HanleGameEnd(js_src, js_dest);
        break;

    default:
        // HandleUnknownCommand(js_src, js_dest);
        break;
    }
}

void GameStateService::HandleFindUserEndPoint(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_FindUserEndPointRes;

    auto gsm = _GSM_Weak.lock();
    if (!gsm)
        return;

    const std::string token = js_src.value("token", "");
    if (token.empty())
        return;

    std::vector<GameStateDef::UserConnectedServiceInfo> userserviceinfos;
    if (!gsm->GetUserEndPoint(token, userserviceinfos))
        return;

    json js_services = json::array();

    for (auto &info : userserviceinfos)
    {
        json js_info = {
            {"id", info.service_id},
            {"type", info.service_type}};
        js_services.push_back(js_info);
    }
    js_dest["result"] = js_services;
}

void GameStateService::HandleUserLogin(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_UserLoginRes;

    auto gsm = _GSM_Weak.lock();
    if (!gsm)
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误";
        return;
    }

    const std::string token = js_src.value("token", "");
    const std::string name = js_src.value("name", "");
    if (token.empty() || name.empty())
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "token或者name 为空!";
        return;
    }

    if (!gsm->user_login(name, token))
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "登录失败！检查是否已登录！";
        return;
    }

    auto u = gsm->get_user_manager().get_user(token);
    if (!u)
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误！";
        return;
    }

    if (js_src.contains("serviceid") && js_src["serviceid"].is_string() &&
        js_src.contains("servicetype") && js_src["servicetype"].is_number_integer())
    {

        std::string serviceid = js_src["serviceid"];
        int type = js_src["servicetype"];

        if (!serviceid.empty())
        {
            UserConnectedServiceInfo info;
            info.service_id = serviceid;
            info.service_type = (ServiceType)type;
            u->connectedservices[serviceid] = std::move(info);
        }
    }

    js_dest["result"] = 1;
}

void GameStateService::HandleUserLogoutService(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_UserLogoutServiceRes;

    bool success = true;
    std::string error_reason;

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    const std::string token = js_src.value("token", "");
    const std::string name = js_src.value("name", "");
    if (success)
    {
        if (token.empty() || name.empty())
        {
            success = false;
            error_reason = "token或者name 为空!";
        }
    }

    RoomPtr room;
    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user)
        {
            success = false;
            error_reason = "用户不存在！";
        }
        room = user->room.lock();
    }

    if (success)
    {
        const std::string serviceid = js_src.value("serviceid", "");
        ServiceType servicetype = (ServiceType)js_src.value("servicetype", 0);

        if (!gsm->get_user_manager().remove_user_from_service(token, serviceid))
        {
            success = false;
            error_reason = "用户不存在或未连接目标id所在的服务！";
        }
    }

    if (success)
    {
        if (room && user->connectedservices.size() <= 0)
            gsm->get_room_manager().player_leave_room(room->room_id, user);
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
}

void GameStateService::HandleGetOnlineLobbyUser(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_GetOnlineLobbyUserRes;

    bool success = true;
    std::string error_reason;

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto users = gsm->get_user_manager().get_all_lobby_users();

    json js_users = json::array();
    for (auto &user : users)
    {
        json js_user = {
            {"token", user->token},
            {"name", user->name}};
        js_users.push_back(js_user);
    }
    js_dest["users"] = js_users;

    if (success)
    {
        js_dest["result"] = 1;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = error_reason;
    }
}

void GameStateService::HandleGetAllLobbyRoom(json &js_src, json &js_dest)
{

    js_dest["command"] = GameStateService_GetAllLobbyRoomRes;

    bool success = true;
    std::string error_reason;

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto rooms = gsm->get_room_manager().get_all_rooms();

    json js_rooms = json::array();
    for (auto &room : rooms)
    {
        json js_room = {
            {"roomid", room->room_id},
            {"roomname", room->room_name},
            {"playercount", room->player_count},
            {"maxplayer", room->max_players},
            {"roomstatus", room->status}};
        js_rooms.push_back(js_room);
    }
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
}

void GameStateService::HandleGetRoomInfo(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_RoomInfoRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("token") || !js_src["token"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("token", "");
    if (success)
    {
        if (token.empty())
        {
            success = false;
            error_reason = "token不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user || user->status != UserStatus::IN_ROOM)
        {
            success = false;
            error_reason = "用户不存在或状态异常！";
        }
    }

    auto room = user->room.lock();
    if (success)
    {
        if (!room)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        json js_room = room->ToJson();
        js_dest["roominfo"] = js_room;
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
}

void GameStateService::HandleCreateRoom(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_CreateRoomRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("token") || !js_src["token"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("token", "");
    if (success)
    {
        if (token.empty())
        {
            success = false;
            error_reason = "token不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user || user->status != UserStatus::IN_LOBBY)
        {
            success = false;
            error_reason = "用户不存在或状态异常！";
        }
    }

    std::string roomid = Tool::GenerateSimpleUuid();
    if (success)
    {
        if (!gsm->create_room(roomid, std::format("[{}]的房间", user->name), 4))
        {
            success = false;
            error_reason = "服务器内部错误！";
            gsm->get_room_manager().remove_room(roomid);
        }
    }

    if (success)
    {
        if (!gsm->join_room(roomid, token))
        {
            success = false;
            error_reason = "服务器内部错误！";
            gsm->get_room_manager().remove_room(roomid);
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
}

void GameStateService::HandleJoinRoom(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_JoinRoomRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("token") ||
            !js_src["roomid"].is_string() ||
            !js_src.contains("roomid") ||
            !js_src["roomid"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("token", "");
    std::string roomid = js_src.value("roomid", "");
    if (success)
    {
        if (token.empty() || roomid.empty())
        {
            success = false;
            error_reason = "token或者roomid不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        if (!gsm->join_room(roomid, token))
        {
            success = false;
            error_reason = "状态异常！";
        }
    }

    if (success)
    {
        if (auto user = gsm->get_user_manager().get_user(token))
        {
            if (auto room = user->room.lock())
            {
                json notify;
                notify["command"] = LobbySubServiceCommand::LobbySubService_RequestRoomInfoRes;
                notify["result"] = 1;
                notify["roominfo"] = room->ToJson();
                NotifyRoommember(room, notify, user);
            }
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
}

void GameStateService::HandleLeaveRoom(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_LeaveRoomRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("token") || !js_src["token"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("token", "");
    if (success)
    {
        if (token.empty())
        {
            success = false;
            error_reason = "token不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user || user->status != UserStatus::IN_ROOM)
        {
            success = false;
            error_reason = "用户不存在或状态异常！";
        }
    }

    auto room = user->room.lock();
    if (success)
    {
        if (!room)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        if (!gsm->leave_room(room->room_id, token))
        {
            success = false;
            error_reason = "状态异常！";
        }
    }

    if (success)
    {
        json notify;
        notify["command"] = LobbySubServiceCommand::LobbySubService_RequestRoomInfoRes;
        notify["result"] = 1;
        notify["roominfo"] = room->ToJson();
        NotifyRoommember(room, notify, user);
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
}

void GameStateService::HandleChangeReadyStatus(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_ChangeReadyStatusRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("token") ||
            !js_src["token"].is_string() ||
            !js_src.contains("status") ||
            !js_src["status"].is_number_integer())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("token", "");
    bool is_ready = js_src.value("status", 0) >= 0;
    if (success)
    {
        if (token.empty())
        {
            success = false;
            error_reason = "token不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user || user->status != UserStatus::IN_ROOM)
        {
            success = false;
            error_reason = "用户不存在或状态异常！";
        }
    }

    auto room = user->room.lock();
    if (success)
    {
        if (!room)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        if (!gsm->change_ready_status(room->room_id, token, is_ready))
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        json notify;
        notify["command"] = LobbySubServiceCommand::LobbySubService_RequestRoomInfoRes;
        notify["result"] = 1;
        notify["roominfo"] = room->ToJson();
        NotifyRoommember(room, notify, user);
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
}

void GameStateService::HandleStartGame(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_StartGameRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("token") || !js_src["token"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("token", "");
    if (success)
    {
        if (token.empty())
        {
            success = false;
            error_reason = "token不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user || user->status != UserStatus::IN_ROOM)
        {
            success = false;
            error_reason = "用户不存在或状态异常！";
        }
    }

    auto room = user->room.lock();
    if (success)
    {
        if (!room)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        bool canstart = true;
        room->members.EnsureCall(
            [&room, &canstart](std::map<std::string, std::shared_ptr<GameStateDef::RoomMemeber>> &map) -> void
            {
                for (auto &pair : map)
                {
                    auto &member = pair.second;
                    auto &membertoken = pair.first;
                    if (member->status != MemberStatus::READY)
                    {
                        if (membertoken == room->room_host_token)
                        {
                            member->status = MemberStatus::READY;
                            continue;
                        }
                        canstart = false;
                        return;
                    }
                }
            });

        if (!canstart)
        {
            success = false;
            error_reason = "存在未准备的玩家！";
        }
    }

    std::shared_ptr<GameServiceHandle> handle;
    if (success)
    {
        if (!Stub_NewGame(room, handle))
        {
            success = false;
            error_reason = "分配服务器失败！";
        }
    }

    if (success)
    {
        room->status = RoomStatus::IN_GAME;
        room->service_handle = handle;

        room->members.EnsureCall(
            [&](std::map<std::string, std::shared_ptr<GameStateDef::RoomMemeber>> &map) -> void
            {
                for (auto &pair : map)
                {
                    auto &member = pair.second;
                    if (auto user = member->userweak.lock())
                        user->status = UserStatus::IN_GAME;
                }
            });

        js_dest["gameid"] = handle->game_id;
        js_dest["gameendpoint"] = handle->service_endpoint.to_json();

        json notify;
        notify["command"] = LobbySubServiceCommand::LobbySubService_RequestStartGameRes;
        notify["result"] = 1;
        notify["gameendpoint"] = handle->service_endpoint.to_json();
        notify["gameid"] = handle->game_id;
        NotifyRoommember(room, notify, user);
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
}

void GameStateService::HanlePlayerLeaveGame(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_PlayerLeaveGameRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("playerid") || !js_src["playerid"].is_string() ||
            !js_src.contains("gameid") || !js_src["gameid"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string token = js_src.value("playerid", "");
    std::string gameid = js_src.value("gameid", "");
    if (success)
    {
        if (token.empty() || gameid.empty())
        {
            success = false;
            error_reason = "playerid或gameid不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    auto user = gsm->get_user_manager().get_user(token);
    if (success)
    {
        if (!user || user->status != UserStatus::IN_GAME)
        {
            success = false;
            error_reason = "用户不存在或状态异常！";
        }
    }

    auto room = user->room.lock();
    if (success)
    {
        if (!room || room->status != RoomStatus::IN_GAME || room->service_handle->game_id != gameid)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        if (!gsm->leave_game(room->room_id, token))
        {
            success = false;
            error_reason = "状态异常！";
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
}

void GameStateService::HanleGameEnd(json &js_src, json &js_dest)
{
    js_dest["command"] = GameStateService_GameEndRes;

    bool success = true;
    std::string error_reason;

    if (success)
    {
        if (!js_src.contains("gameid") || !js_src["gameid"].is_string())
        {
            success = false;
            error_reason = "请求数据缺失";
        }
    }

    std::string gameid = js_src.value("gameid", "");
    if (success)
    {
        if (gameid.empty())
        {
            success = false;
            error_reason = "gameid不能为空";
        }
    }

    auto gsm = _GSM_Weak.lock();
    if (success)
    {
        if (!gsm)
        {
            success = false;
            error_reason = "服务器内部错误！";
        }
    }

    if (success)
    {
        if (!gsm->game_end(gameid))
        {
            success = false;
            error_reason = "状态异常！";
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
}

bool GameStateService::Stub_NewGame(std::shared_ptr<Room> room, std::shared_ptr<GameServiceHandle> &handle)
{
    std::vector<ServiceInfo> services;
    if (!_servcieDiscoverClient.GetAvailableServiceInfo(ServiceType::GAME, services) || services.empty())
        return false;

    json js_request, js_response;
    js_request["command"] = GameServiceCommand::GameService_NewGame;

    json js_players = json::array();
    room->members.EnsureCall(
        [&](std::map<std::string, std::shared_ptr<GameStateDef::RoomMemeber>> &map) -> void
        {
            for (auto &pair : map)
            {
                auto &member = pair.second;
                auto token = pair.first;
                std::string playerId = token;
                std::string name = member->name;

                js_players.push_back({{"playerid", playerId},
                                      {"name", name}});
            }
        });

    js_request["players"] = js_players;

    JsonProtocolClient client;

    for (auto &service : services)
    {
        if (service.endpoint.ip.empty() ||
            service.stub_endpoint.ip.empty() ||
            service.endpoint.port == 0 ||
            service.stub_endpoint.port == 0)
            continue;

        if (!client.Connect(service.stub_endpoint.ip, service.stub_endpoint.port))
            continue;

        if (Stub_Request(client, js_request, js_response))
        {
            handle = std::make_shared<GameServiceHandle>();
            handle->service_id = service.service_id;
            handle->service_endpoint = service.endpoint;
            handle->service_stub_endpoint = service.stub_endpoint;

            if (!js_response.contains("gameid") || !js_response["gameid"].is_string())
                return false;

            handle->game_id = js_response.value("gameid", "");
            if (handle->game_id.empty())
                return false;

            return true;
        }
    }

    return false;
}

bool GameStateService::NotifyRoommember(std::shared_ptr<Room> room, json &js, std::shared_ptr<User> excludeuser)
{
    if (!room)
        return false;

    bool result = true;
    if (!room->members.IsEmpty())
    {
        auto usertokens = room->members.GetKeys();
        for (auto &token : usertokens)
        {
            if (excludeuser && excludeuser->token == token)
                continue;
            if (!_delegator.DelegateMessage(token, js))
                result = false;
        }
    }
    return result;
}

bool GameStateService::Stub_Request(JsonProtocolClient &client, const json &js_request, json &response)
{
    if (!client.Request(js_request, response))
        return false;

    if (!response.contains("result") || !response["result"].is_number_integer())
        return false;

    int result = response["result"];
    if (result < 0)
    {
        std::string reason = response.value("reason", "");
        std::cout << "Stub Request fail! command =" << js_request.value("command", 0) << " reason:" << reason << '\n';
        return false;
    }
    return true;
}
