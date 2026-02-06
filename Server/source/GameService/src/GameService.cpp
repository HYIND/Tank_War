#include "GameService.h"
#include "command.h"
#include "tools.h"
#include "GameDataDef.h"

using namespace GameServiceCommand;

GameService::GameService()
{
    _gameStateStub = std::make_shared<JsonProtocolClient>();
    _serviceinfo = std::make_shared<ServiceInfo>(Tool::GenerateSimpleUuid(), ServiceType::GAME);
    _service_server->SetCallbackSessionEstablish(std::bind(&GameService::OnSessionEstablish, this, std::placeholders::_1));
}

GameService::~GameService()
{
}

void GameService::SetGameStateEndPoint(const std::string &IP, int Port)
{
    _gameStateStubIP = IP;
    _gameStateStubPort = Port;
}

bool GameService::Start()
{
    bool result = _gameStateStub->Connect(_gameStateStubIP, _gameStateStubPort) && BaseService::Start();
    if (result)
    {
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

void GameService::OnSessionEstablish(JsonProtocolSession session)
{
    _service_server->SetCallBackRecvJsonMessage(session, std::bind(&GameService::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _service_server->SetCallBackRecvJsonRequest(session, std::bind(&GameService::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _service_server->SetCallBackCloseConnect(session, std::bind(&GameService::OnSessionClose, this, std::placeholders::_1));
}

void GameService::OnRecvMessage(JsonProtocolSession session, json &src)
{
    json dest;
    ProcessMsg(session, src, dest);
    if (!dest.is_null())
        session.AsyncSendJson(dest);
}

void GameService::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    ProcessMsg(session, src, dest);
}

void GameService::OnSessionClose(JsonProtocolSession session)
{
}

std::vector<ServiceInfo> GameService::GetServiceInfo()
{
    return std::vector<ServiceInfo>{*_serviceinfo};
}

void GameService::OnStubRequest(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    if (command == GameService_NewGame)
    {
        Stub_ProcessNewGame(js_src, js_dest);
    }
}

void GameService::SendToPlayers(const GameID &gameId, const std::vector<PlayerID> &playerIds, const json &message)
{
    std::shared_ptr<GameInstance> instance;
    if (!_GameIdToGameInstance.Find(gameId, instance) || !instance)
        return;

    auto game_playerids = instance->GetAllPlayerIds();
    for (auto &playerid : playerIds)
    {
        if (std::find(game_playerids.begin(), game_playerids.end(), playerid) == game_playerids.begin())
            continue;

        JsonProtocolSession session;
        if (_SessionToplayerId.FindByRight(playerid, session) && session.isValid())
            session.AsyncSendJson(message);
    }
}

void GameService::SendToPlayer(const GameID &gameId, const PlayerID &playerId, const json &message)
{
    std::shared_ptr<GameInstance> instance;
    if (!_GameIdToGameInstance.Find(gameId, instance) || !instance)
        return;

    auto game_playerids = instance->GetAllPlayerIds();
    if (std::find(game_playerids.begin(), game_playerids.end(), playerId) == game_playerids.begin())
        return;

    JsonProtocolSession session;
    if (_SessionToplayerId.FindByRight(playerId, session) && session.isValid())
        session.AsyncSendJson(message);
}

void GameService::BroadcastToGame(const GameID &gameId, const json &message, const std::vector<PlayerID> &exclude)
{
    std::shared_ptr<GameInstance> instance;
    if (!_GameIdToGameInstance.Find(gameId, instance) || !instance)
        return;

    auto game_playerids = instance->GetAllPlayerIds();
    for (auto &playerid : game_playerids)
    {
        if (std::find(exclude.begin(), exclude.end(), playerid) != exclude.begin())
            continue;

        JsonProtocolSession session;
        if (_SessionToplayerId.FindByRight(playerid, session) && session.isValid())
            session.AsyncSendJson(message);
    }
}

void GameService::ProcessMsg(JsonProtocolSession &session, json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    if (command == GameService_JoinGame)
    {
        ProcessPlayerJoin(session, js_src, js_dest);
        return;
    }

    PlayerID playerId;
    if (!Vertify(session, playerId))
        return;

    if (command == GameService_InputToGame)
    {
        ProcessPlayerInput(playerId, js_src, js_dest);
    }
    else if (command == GameService_LeaveGame)
    {
        ProcessPlayerLeave(playerId, js_src, js_dest);
    }
}

bool GameService::Vertify(const JsonProtocolSession &session, PlayerID &playerid)
{
    return _SessionToplayerId.FindByLeft(session, playerid) && !playerid.empty();
}

void GameService::Stub_ProcessNewGame(json &js_src, json &js_dest)
{
    js_dest["command"] = GameService_NewGameRes;

    try
    {
        if (!js_src.contains("players") ||
            !js_src["players"].is_array())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "players为空！";
            return;
        }

        // 解析新游戏请求
        json players = js_src["players"];

        GameID gameId = Tool::GenerateSimpleUuid();

        if (gameId.empty())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "gameId为空！";
            return;
        }

        // 检查游戏是否已存在
        {
            if (_GameIdToGameInstance.Exist(gameId))
            {
                js_dest["result"] = -1;
                js_dest["reason"] = "服务器内部错误！";
                return;
            }
        }

        // 创建新游戏实例
        auto game = CreateNewGame(gameId);

        // 添加玩家
        for (const auto &player : players)
        {
            if (!player.contains("playerid") ||
                !player["playerid"].is_string() ||
                !player.contains("name") ||
                !player["name"].is_string())
                continue;

            auto gp = std::make_shared<GamePlayer>();
            gp->playerid = player["playerid"];
            gp->name = player.value("name", "");
            game->AddPlayer(gp);
        }

        // 初始化游戏
        if (!game->Initialize())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "游戏初始化失败！";
            return;
        }

        // 保存游戏实例
        {
            _GameIdToGameInstance[gameId] = game;
            game->Start();
        }

        // 返回成功响应
        js_dest["result"] = 1;
        js_dest["gameid"] = gameId;
        js_dest["endpoint"] = _serviceinfo->endpoint.to_json();
        js_dest["serviceid"] = _serviceinfo->service_id;
        js_dest["timestamp"] = Tool::GetTimestampSecond();

        // LOG_INFO("New game created: {}, players: {}",
        //          gameId, players.size());
    }
    catch (const std::exception &e)
    {
        // LOG_ERROR("ProcessNewGame error: {}", e.what());
        js_dest["result"] = -1;
        js_dest["reason"] = e.what();
    }
}

void GameService::ProcessPlayerJoin(const JsonProtocolSession &session, json &js_src, json &js_dest)
{
    js_dest["command"] = GameService_JoinGameRes;

    try
    {

        if (!js_src.contains("gameid") ||
            !js_src["gameid"].is_string() ||
            !js_src.contains("playerid") ||
            !js_src["playerid"].is_string())
        {

            js_dest["result"] = -1;
            js_dest["reason"] = "参数缺失！";
            return;
        }

        GameID gameid = js_src.value("gameid", "");
        PlayerID playerid = js_src.value("playerid", "");

        if (gameid.empty() || playerid.empty())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "参数不能为空！";
            return;
        }

        auto guard = _GameIdToGameInstance.MakeLockGuard();
        if (!_GameIdToGameInstance.Exist(gameid))
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "Gameid不存在！";
            return;
        }

        auto gameinstance = _GameIdToGameInstance[gameid];
        if (!gameinstance)
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "服务器内部错误！";
            return;
        }

        if (auto gameplayer = gameinstance->GetPlayer(playerid))
        {
            _SessionToplayerId.InsertOrUpdate(session, playerid);
            _PlayerIdToGameId.EnsureInsert(playerid, gameid);
        }

        js_dest["result"] = 1;
    }
    catch (const std::exception &e)
    {
        // LOG_ERROR("ProcessNewGame error: {}", e.what());
        js_dest["result"] = -1;
        js_dest["reason"] = e.what();
    }
}

void GameService::ProcessPlayerInput(const PlayerID &playerId, json &js_src, json &js_dest)
{
    js_dest["command"] = GameService_InputToGameRes;

    GameID gameId;
    if (!_PlayerIdToGameId.Find(playerId, gameId) || gameId.empty())
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "非法的playerid";
        return;
    }

    std::shared_ptr<GameInstance> game_instance;
    if (!_GameIdToGameInstance.Find(gameId, game_instance) || !game_instance)
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "非法的gameid";
        return;
    }

    game_instance->ProcessPlayerInput(playerId, js_src);
}

void GameService::ProcessPlayerLeave(const PlayerID &playerId, json &js_src, json &js_dest)
{
}

void GameService::ProcessGameEnd(json &js_src, json &js_dest)
{
}

std::shared_ptr<GameInstance> GameService::CreateNewGame(const GameID &gameId)
{
    auto game = std::make_shared<GameInstance>(gameId, 1);
    auto sender = std::make_shared<NetworkMessageSender>(gameId);
    sender->SetGameService(shared_from_this());
    game->SetNetworkMessageSender(sender);
    return game;
}

void GameService::RemoveGame(const GameID &gameId)
{
    _GameIdToGameInstance.Erase(gameId);
}

std::shared_ptr<GameInstance> GameService::FindGame(const GameID &gameId)
{
    std::shared_ptr<GameInstance> instance;
    if (_GameIdToGameInstance.Find(gameId, instance))
        return instance;

    return std::shared_ptr<GameInstance>(nullptr);
}
