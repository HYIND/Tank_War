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
    virtual bool Start();

    void SetGameStateManager(std::shared_ptr<GameStateManager> gsm);

public:
    void OnSessionEstablish(JsonProtocolSession session);
    void OnRecvMessage(JsonProtocolSession session, json &src);
    void OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    void OnSessionClose(JsonProtocolSession session);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();
    virtual void OnStubRequest(json &js_src, json &js_dest);

private:
    void ProcessMsg(json &js_src, json &js_dest);

    void HandleFindUserEndPoint(json &js_src, json &js_dest);

    void HandleUserLogin(json &js_src, json &js_dest);
    void HandleUserLogoutService(json &js_src, json &js_dest);

    void HandleGetOnlineLobbyUser(json &js_src, json &js_dest);
    void HandleGetAllLobbyRoom(json &js_src, json &js_dest);

    void HandleGetRoomInfo(json &js_src, json &js_dest);
    void HandleCreateRoom(json &js_src, json &js_dest);
    void HandleJoinRoom(json &js_src, json &js_dest);
    void HandleLeaveRoom(json &js_src, json &js_dest);
    void HandleChangeReadyStatus(json &js_src, json &js_dest);
    void HandleStartGame(json &js_src, json &js_dest);

private:
    bool Stub_NewGame(std::shared_ptr<Room> room,std::shared_ptr<GameServiceHandle>& handle);
    bool NotifyRoommember(std::shared_ptr<Room> room, json &js, std::shared_ptr<User> excludeuser = nullptr);

private:
    bool Stub_Request(JsonProtocolClient &client, const json &js_request, json &response);

private:
    std::weak_ptr<GameStateManager> _GSM_Weak;
    std::shared_ptr<ServiceInfo> _serviceinfo;

    ServiceDiscoveryClient _servcieDiscoverClient;
    MessageDelegator _delegator;
};