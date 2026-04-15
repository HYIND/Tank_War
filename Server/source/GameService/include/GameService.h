#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "BaseService.h"
#include "GameInstance.h"
#include "GameSession.h"
#include "ServiceDiscoveryClient.h"
#include "BiDirectionalMap.h"
#include "SafeStl.h"

using namespace ServiceRegistryDataDef;

class GameInstance;
class GameService : public BaseService, public std::enable_shared_from_this<GameService>
{

public:
    GameService();
    ~GameService();

    void SetGameStateEndPoint(const std::string &IP, int Port);
    void SetServiceReportEndPoint(const std::string &IP, int Port);
    void SetStubReportEndPoint(const std::string &IP, int Port);

    virtual Task<bool> Start();

public:
    Task<void> OnSessionEstablish(JsonProtocolSession session);
    Task<void> OnRecvMessage(JsonProtocolSession session, json &src);
    Task<void> OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    Task<void> OnSessionClose(JsonProtocolSession session);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();
    virtual Task<void> OnStubRequest(json &js_src, json &js_dest);

public:
    void SendToPlayers(const GameID &gameId, const std::vector<PlayerID> &playerIds, const json &message);
    void SendToPlayer(const GameID &gameId, const PlayerID &playerId, const json &message);
    void BroadcastToGame(const GameID &gameId, const json &message, const std::vector<PlayerID> &exclude);
    Task<void> GameOver(const GameID &gameId);

private:
    Task<void> ProcessMsg(JsonProtocolSession &session, json &js_src, json &js_dest);

    bool Vertify(const JsonProtocolSession &session, PlayerID &playerid);

    Task<void> ProcessPlayerJoin(const JsonProtocolSession &session, json &js_src, json &js_dest);
    Task<void> ProcessPlayerInput(const PlayerID &playerId, json &js_src, json &js_dest);
    Task<void> ProcessPlayerLeave(const PlayerID &playerId, json &js_src, json &js_dest);

private:
    Task<void> OnStub_ProcessNewGame(json &js_src, json &js_dest);

private:
    Task<bool> Stub_PlayerLeaveGame(const PlayerID &playerId, const GameID &gameId);
    Task<bool> Stub_GameEnd(const GameID &gameId);

    Task<bool> Stub_Request(JsonProtocolClient &client, const json &js_request, json &response);

private:
    std::shared_ptr<GameInstance> CreateNewGame(const GameID &gameId);
    void RemoveGame(const GameID &gameId);
    std::shared_ptr<GameInstance> FindGame(const GameID &gameId);

private:
    std::shared_ptr<JsonProtocolClient> _gameStateStub;
    std::string _gameStateStubIP;
    int _gameStateStubPort;

    std::string _serviceReportIP;
    int _serviceReportPort;

    std::string _stubReportIP;
    int _stubReportPort;

    std::shared_ptr<ServiceInfo> _serviceinfo;

    // 游戏实例管理
    SafeUnorderedMap<GameID, std::shared_ptr<GameInstance>, CoroCriticalSectionLock> _GameIdToGameInstance;

    // 玩家会话管理
    SafeBiDirectionalMap<JsonProtocolSession, PlayerID, CoroCriticalSectionLock> _SessionToplayerId;
    SafeUnorderedMap<PlayerID, GameID, CoroCriticalSectionLock> _PlayerIdToGameId;

    ServiceDiscoveryClient _servcieDiscoverClient;
};