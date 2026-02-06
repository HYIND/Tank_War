#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Manager/MapManager.h"
#include "GameDataDef.h"
#include "ECS/Core/World.h"
#include "SafeStl.h"
#include "NetworkMessageSender.h"

class NetworkMessageSender;
class GameInstance
{
public:
    enum class State
    {
        INITIALIZING, // 初始化中
        WAITING,      // 等待玩家加入
        RUNNING,      // 游戏中
        PAUSED,       // 暂停
        ENDED         // 结束
    };

    GameInstance(const GameID &gameId, MapID mapid);
    ~GameInstance();

    void SetNetworkMessageSender(std::shared_ptr<NetworkMessageSender> sender);

    // 游戏生命周期
    bool Initialize();
    void Start();
    void Stop();
    void End();

    // 玩家管理
    bool AddPlayer(std::shared_ptr<GamePlayer> player);
    bool RemovePlayer(const PlayerID &playerId);
    std::shared_ptr<GamePlayer> GetPlayer(const PlayerID &playerId);
    std::vector<PlayerID> GetAllPlayerIds() const;

    // 状态查询
    State GetState() const { return _state; }
    std::string GetGameId() const { return _gameId; }
    int GetPlayerCount() const { return _PlayerIdToGamePlayer.Size(); }

    // 游戏逻辑
    void ProcessPlayerInput(const PlayerID &playerId, const json &input);
    json GetGameState() const;
    json GetPlayerView(const PlayerID &playerId) const;

private:
    void GameLoop();
    void LoadMapInfoToWorld();

    void BroadCaseGameState();

private:
    GameID _gameId;
    State _state;

    // 玩家管理
    SafeUnorderedMap<PlayerID, std::shared_ptr<GamePlayer>> _PlayerIdToGamePlayer;
    mutable std::mutex _playersMutex;

    // 游戏状态
    json _gameState;
    mutable std::mutex _stateMutex;

    bool _stop = true;

    std::shared_ptr<NetworkMessageSender> _sender;

    MapID _mapid;
    std::shared_ptr<World> _world;

    std::shared_ptr<std::thread> _worldThread;
};