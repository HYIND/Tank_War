#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "LobbyDataDef.h"
#include "LoginSubService.h"
#include "ServiceRegistryData.h"
#include "MessageDelegator.h"

using namespace ServiceRegistryDataDef;

class LoginSubService;
class LobbySubService
{
public:
    LobbySubService();
    ~LobbySubService();

public:
    Task<void> OnRecvMessage(JsonProtocolSession session, json &src);
    Task<void> OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    Task<void> OnSessionClose(JsonProtocolSession session);

    void SetLoginSubService(std::shared_ptr<LoginSubService> service);
    void SetServer(std::shared_ptr<JsonProtocolServer> m);
    void SetGameStateStub(std::shared_ptr<JsonProtocolClient> stub);
    void SetServiceInfo(std::shared_ptr<ServiceInfo> info);
    void SetMessageDelegator(std::shared_ptr<MessageDelegator> delegator);

public:
    Task<bool> UserEnter(std::shared_ptr<LobbyUser> u);
    Task<void> OnStubRequest(json &js_src, json &js_dest);

private:
    Task<void> ProcessMsg(JsonProtocolSession session, json &src, json &dest);
    Task<void> ProcessLogout(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

    Task<void> ProcessHallMsg(std::shared_ptr<LobbyUser> sender, json &js_src, json &js_dest);
    Task<void> ProcessRoomMsg(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessPrivateMsg(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

    Task<void> ProcessRequestLobbyUser(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessRequestLobbyRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

    Task<void> ProcessRequestRoomInfo(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessRequestCreateRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessRequestJoinRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessRequestLeaveRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessRequestChangeReadyStatus(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    Task<void> ProcessRequestStartGame(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

private:
    Task<bool> Stub_Login(std::shared_ptr<LobbyUser> user);
    Task<bool> Stub_Logout(std::shared_ptr<LobbyUser> user);

    Task<bool> Stub_GetOnlineLobbyUser(std::vector<LobbyUser> &users);
    Task<bool> Stub_GetAllLobbyRoom(json &js_rooms);

    Task<bool> Stub_RoomInfo(std::shared_ptr<LobbyUser> user, json &js_roominfo);
    Task<bool> Stub_CreateRoom(std::shared_ptr<LobbyUser> user);
    Task<bool> Stub_JoinRoom(std::shared_ptr<LobbyUser> user, const std::string &roomid);
    Task<bool> Stub_LeaveRoom(std::shared_ptr<LobbyUser> user);
    Task<bool> Stub_ChangeReadyStatus(std::shared_ptr<LobbyUser> user, int status);
    Task<bool> Stub_StartGame(std::shared_ptr<LobbyUser> user, NetworkEndpoint &endpoint, std::string &gameid);

private:
    Task<bool> Stub_Request(const json &js_request, json &response);
    Task<bool> DelegateDispatch(const std::string &user_token, const json &js);

private:
    std::shared_ptr<JsonProtocolClient> _gameStateStub;

    std::shared_ptr<JsonProtocolServer> _server;
    std::weak_ptr<LoginSubService> _weakloginSubservice;

    std::shared_ptr<ServiceInfo> _serviceinfo;

    SafeUnorderedMap<std::string, std::shared_ptr<LobbyUser>> _tokenToUser;
    SafeUnorderedMap<JsonProtocolSession, std::shared_ptr<LobbyUser>> _sessionToUser;

    std::shared_ptr<MessageDelegator> _delegator;
};