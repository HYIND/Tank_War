#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "BaseService.h"
#include "GameInstance.h"
#include "GameSession.h"
#include "SafeStl.h"
#include "BiDirectionalMap.h"
#include "ServiceDiscoveryClient.h"

using namespace ServiceRegistryDataDef;

class GameInstance;
class GameService : public BaseService, public std::enable_shared_from_this<GameService>
{

public:
    GameService();
    ~GameService();

    void SetGameStateEndPoint(const std::string &IP, int Port);

    virtual bool Start();

public:
    void OnSessionEstablish(JsonProtocolSession session);
    void OnRecvMessage(JsonProtocolSession session, json &src);
    void OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    void OnSessionClose(JsonProtocolSession session);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();
    virtual void OnStubRequest(json &js_src, json &js_dest);

public:
    void SendToPlayers(const GameID &gameId, const std::vector<PlayerID> &playerIds, const json &message);
    void SendToPlayer(const GameID &gameId, const PlayerID &playerId, const json &message);
    void BroadcastToGame(const GameID &gameId, const json &message, const std::vector<PlayerID> &exclude);
    void GameOver(const GameID &gameId);

private:
    void ProcessMsg(JsonProtocolSession &session, json &js_src, json &js_dest);

    bool Vertify(const JsonProtocolSession &session, PlayerID &playerid);

    void ProcessPlayerJoin(const JsonProtocolSession &session, json &js_src, json &js_dest);
    void ProcessPlayerInput(const PlayerID &playerId, json &js_src, json &js_dest);
    void ProcessPlayerLeave(const PlayerID &playerId, json &js_src, json &js_dest);

private:
    void OnStub_ProcessNewGame(json &js_src, json &js_dest);

private:
    bool Stub_PlayerLeaveGame(const PlayerID &playerId, const GameID &gameId);
    bool Stub_GameEnd(const GameID &gameId);

    bool Stub_Request(JsonProtocolClient &client, const json &js_request, json &response);

private:
    std::shared_ptr<GameInstance> CreateNewGame(const GameID &gameId);
    void RemoveGame(const GameID &gameId);
    std::shared_ptr<GameInstance> FindGame(const GameID &gameId);

private:
    std::shared_ptr<JsonProtocolClient> _gameStateStub;
    std::string _gameStateStubIP;
    int _gameStateStubPort;

    std::shared_ptr<ServiceInfo> _serviceinfo;

    // 游戏实例管理
    SafeUnorderedMap<GameID, std::shared_ptr<GameInstance>> _GameIdToGameInstance;

    // 玩家会话管理
    SafeBiDirectionalMap<JsonProtocolSession, PlayerID> _SessionToplayerId;
    SafeUnorderedMap<PlayerID, GameID> _PlayerIdToGameId;

    ServiceDiscoveryClient _servcieDiscoverClient;
};