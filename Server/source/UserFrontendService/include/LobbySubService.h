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
    void OnRecvMessage(JsonProtocolSession session, json &src);
    void OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    void OnSessionClose(JsonProtocolSession session);

    void SetLoginSubService(std::shared_ptr<LoginSubService> service);
    void SetServer(std::shared_ptr<JsonProtocolServer> m);
    void SetGameStateStub(std::shared_ptr<JsonProtocolClient> stub);
    void SetServiceInfo(std::shared_ptr<ServiceInfo> info);
    void SetMessageDelegator(std::shared_ptr<MessageDelegator> delegator);

public:
    bool UserEnter(std::shared_ptr<LobbyUser> u);
    void OnStubRequest(json &js_src, json &js_dest);

private:
    void ProcessMsg(JsonProtocolSession session, json &src, std::vector<json> &dest);
    void ProcessLogout(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

    void ProcessHallMsg(std::shared_ptr<LobbyUser> sender, json &js_src, json &js_dest);
    void ProcessRoomMsg(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessPrivateMsg(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

    void ProcessRequestLobbyUser(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessRequestLobbyRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

    void ProcessRequestRoomInfo(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessRequestCreateRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessRequestJoinRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessRequestLeaveRoom(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessRequestChangeReadyStatus(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);
    void ProcessRequestStartGame(std::shared_ptr<LobbyUser> user, json &js_src, json &js_dest);

private:
    bool Stub_Login(std::shared_ptr<LobbyUser> user);
    bool Stub_Logout(std::shared_ptr<LobbyUser> user);

    bool Stub_GetOnlineLobbyUser(std::vector<LobbyUser> &users);
    bool Stub_GetAllLobbyRoom(json &js_rooms);

    bool Stub_RoomInfo(std::shared_ptr<LobbyUser> user, json &js_roominfo);
    bool Stub_CreateRoom(std::shared_ptr<LobbyUser> user);
    bool Stub_JoinRoom(std::shared_ptr<LobbyUser> user, const std::string &roomid);
    bool Stub_LeaveRoom(std::shared_ptr<LobbyUser> user);
    bool Stub_ChangeReadyStatus(std::shared_ptr<LobbyUser> user, int status);
    bool Stub_StartGame(std::shared_ptr<LobbyUser> user, NetworkEndpoint &endpoint, std::string &gameid);

private:
    bool Stub_Request(const json &js_request, json &response);
    bool DelegateDispatch(const std::string &user_token, const json &js);

private:
    std::shared_ptr<JsonProtocolClient> _gameStateStub;

    std::shared_ptr<JsonProtocolServer> _server;
    std::weak_ptr<LoginSubService> _weakloginSubservice;

    std::shared_ptr<ServiceInfo> _serviceinfo;

    SafeUnorderedMap<std::string, std::shared_ptr<LobbyUser>> _tokenToUser;
    SafeUnorderedMap<JsonProtocolSession, std::shared_ptr<LobbyUser>> _sessionToUser;

    std::shared_ptr<MessageDelegator> _delegator;
};