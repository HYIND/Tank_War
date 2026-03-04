#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "BaseService.h"
#include "GameStateManager.h"
#include "ServiceDiscoveryClient.h"
#include "MessageDelegator.h"

using namespace ServiceRegistryDataDef;

class GameStateService : public BaseService, public std::enable_shared_from_this<GameStateService>
{

public:
    GameStateService();
    ~GameStateService();
    virtual Task<bool> Start();

    void SetGameStateManager(std::shared_ptr<GameStateManager> gsm);

public:
    Task<void> OnSessionEstablish(JsonProtocolSession session);
    Task<void> OnRecvMessage(JsonProtocolSession session, json &src);
    Task<void> OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    Task<void> OnSessionClose(JsonProtocolSession session);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();
    virtual Task<void> OnStubRequest(json &js_src, json &js_dest);

private:
    Task<void> ProcessMsg(json &js_src, json &js_dest);

    Task<void> HandleFindUserEndPoint(json &js_src, json &js_dest);

    Task<void> HandleUserLogin(json &js_src, json &js_dest);
    Task<void> HandleUserLogoutService(json &js_src, json &js_dest);

    Task<void> HandleGetOnlineLobbyUser(json &js_src, json &js_dest);
    Task<void> HandleGetAllLobbyRoom(json &js_src, json &js_dest);

    Task<void> HandleGetRoomInfo(json &js_src, json &js_dest);
    Task<void> HandleCreateRoom(json &js_src, json &js_dest);
    Task<void> HandleJoinRoom(json &js_src, json &js_dest);
    Task<void> HandleLeaveRoom(json &js_src, json &js_dest);
    Task<void> HandleChangeReadyStatus(json &js_src, json &js_dest);
    Task<void> HandleStartGame(json &js_src, json &js_dest);

    Task<void> HanlePlayerLeaveGame(json &js_src, json &js_dest);
    Task<void> HanleGameEnd(json &js_src, json &js_dest);

private:
    Task<bool> Stub_NewGame(std::shared_ptr<Room> room,std::shared_ptr<GameServiceHandle>& handle);
    Task<bool> NotifyRoommember(std::shared_ptr<Room> room, json &js, std::shared_ptr<User> excludeuser = nullptr);

private:
    Task<bool> Stub_Request(JsonProtocolClient &client, const json &js_request, json &response);

private:
    std::weak_ptr<GameStateManager> _GSM_Weak;
    std::shared_ptr<ServiceInfo> _serviceinfo;

    ServiceDiscoveryClient _servcieDiscoverClient;
    MessageDelegator _delegator;
};