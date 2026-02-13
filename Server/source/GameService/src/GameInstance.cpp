#include "GameInstance.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Systems/AllSystem.h"
#include "ECS/Factory/TankFactory.h"
#include "ECS/Factory/WallFactory.h"
#include "ECS/Factory/PropFactory.h"
#include "ECS/Factory/BulletFactory.h"
#include "Helper/DynamicFpsController.h"
#include "command.h"
#include "Config.h"

using namespace GameServiceCommand;

void GameInstance::LoadMapInfoToWorld()
{
	auto mapinfo = MapManager::Instance()->getMap(_mapid);

	static std::vector<TankProperty::TankOwner> avaliableplayer = { TankProperty::TankOwner::PLAYER1, TankProperty::TankOwner::PLAYER2 };

	auto playerid_list = _PlayerIdToGamePlayer.GetKeys();
	// 生成玩家坦克
	for (int i = 0; i < mapinfo.tankbirthinfos.size() && i < avaliableplayer.size() && i < playerid_list.size(); i++)
	{
		auto& tankInfo = mapinfo.tankbirthinfos[i];

		// 使用TankFactory创建玩家坦克
		Entity tank = TankFactory::CreateServerTank(
			*_world,
			avaliableplayer[i],
			playerid_list[i],
			tankInfo.position.x,
			tankInfo.position.y,
			tankInfo.width,
			tankInfo.height);

		// 设置旋转和视觉状态
		if (auto* trans = tank.tryGetComponent<Transform>())
			trans->rotation = tankInfo.rotation;

		if (auto* visual = tank.tryGetComponent<TankVisual>())
			visual->visualstate = tankInfo.visual;
	}

	// 生成AI坦克
	for (auto& aiTankInfo : mapinfo.aitankbirthinfos)
	{
		Entity aiTank = TankFactory::CreateServerTank(
			*_world,
			TankProperty::TankOwner::AI,
			"",
			aiTankInfo.position.x,
			aiTankInfo.position.y,
			aiTankInfo.width,
			aiTankInfo.height);

		if (auto* trans = aiTank.tryGetComponent<Transform>())
			trans->rotation = aiTankInfo.rotation;

		if (auto* visual = aiTank.tryGetComponent<TankVisual>())
			visual->visualstate = aiTankInfo.visual;
	}

	// 生成道具
	for (auto& propInfo : mapinfo.propbirthinfos)
	{
		Entity prop = PropFactory::CreateServerProp(
			*_world,
			propInfo.type,
			propInfo.duration,
			propInfo.position.x,
			propInfo.position.y,
			propInfo.rotation,
			propInfo.width,
			propInfo.height,
			60.f);
	}

	for (auto& wallInfo : mapinfo.wallbirthinfos)
	{
		Entity wall = WallFactory::CreateServerWall(
			*_world,
			wallInfo.type,
			wallInfo.position.x,
			wallInfo.position.y,
			wallInfo.rotation,
			wallInfo.width,
			wallInfo.height,
			wallInfo.health);

		if (auto* trans = wall.tryGetComponent<Transform>())
			trans->rotation = wallInfo.rotation;

		if (auto* health = wall.tryGetComponent<Health>())
		{
			health->maxHealth = wallInfo.health;
			health->currentHealth = wallInfo.health;
		}
	}
}

void GameInstance::BroadCaseGameState()
{
	if (!_sender)
		return;

	auto& sender = _sender;

	if (auto sync = _world->getSystem<ServerStateSyncStstem>())
	{
		auto& gamestate = sync->GetGameState();
		if (gamestate.hasConsume)
			return;

		gamestate.hasConsume = true;

		json js;
		js["command"] = GameService_BroadCastGameState;
		js["gamestate"] = gamestate.toJson();

		sender->Broadcast(js);
	}
}

GameInstance::GameInstance(const GameID& gameId, MapID mapid)
	: _gameId(gameId), _mapid(mapid), _state(State::RUNNING)
{
}

GameInstance::~GameInstance()
{
}

void GameInstance::SetNetworkMessageSender(std::shared_ptr<NetworkMessageSender> sender)
{
	_sender = sender;
}

void GameInstance::SetGameService(std::shared_ptr<GameService> service)
{
	_service = service;
}

bool GameInstance::Initialize()
{
	_world = std::make_shared<World>();

	// 通过World注册系统
	auto& inputSystem = _world->registerSystem<ServerInputSystem>();
	auto& velocityControlSystem = _world->registerSystem<VelocityControlSystem>();
	auto& movementSystem = _world->registerSystem<MovementSystem>();
	auto& weaponSystem = _world->registerSystem<WeaponSystem>();
	auto& bulletSystem = _world->registerSystem<BulletSystem>();
	auto& lifetimeSystem = _world->registerSystem<LifetimeSystem>();
	auto& physicsSystem = _world->registerSystem<PhysicsSystem>();
	auto& destroySystem = _world->registerSystem<DestroySystem>();
	auto& healthSystem = _world->registerSystem<HealthSystem>();
	auto& wallSystem = _world->registerSystem<WallSystem>();
	auto& propSystem = _world->registerSystem<PropSystem>();
	auto& tankSystem = _world->registerSystem<TankSystem>();
	auto& syncSystem = _world->registerSystem<ServerStateSyncStstem>();

	lifetimeSystem.setPriority(10000);
	inputSystem.setPriority(1000);
	velocityControlSystem.setPriority(500);
	movementSystem.setPriority(500);
	physicsSystem.setPriority(300);
	destroySystem.setPriority(-9000);
	syncSystem.setPriority(-20000);

	float syncfps = Config("../config/config.ini").Read(std::string("ServerSyncFps"), 60.0);

	syncSystem.SetSyncFps(syncfps);

	LoadMapInfoToWorld();

	Entity map_boundary = _world->createEntityWithTag<TagGameBoundary>();
	auto& boundary = map_boundary.addComponent<BoundaryPhysisc>(MapBoundary::left, MapBoundary::top, MapBoundary::right, MapBoundary::bottom);

	return true;
}

void GameInstance::Start()
{
	if (!_world)
		return;

	uint64_t gametimelimit = Config("../config/config.ini").Read(std::string("GameTimeLimit"), 300);
	if (auto* system_ptr = _world->getSystem<CheckGameOverSystem>())
	{
		system_ptr->setPriority(999999);
		system_ptr->SetInstance(shared_from_this());
		system_ptr->SetGameTimeLimit(gametimelimit);
	}
	else
	{
		auto& system = _world->registerSystem<CheckGameOverSystem>(999999);
		system.SetInstance(shared_from_this());
		system.SetGameTimeLimit(gametimelimit);
	}

	// 启动世界
	_world->setLogicDeltaTime(1000.f / 100.f);
	_world->setFixedDeltaTime(1000.f / 60.f);
	_world->start();
	_stop = false;
	_worldThread = std::make_shared<std::thread>(&GameInstance::GameLoop, this);
}

void GameInstance::Stop()
{
	if (!_world)
		return;

	_world->stop();
	_stop = true;
	if (_worldThread && _worldThread->joinable())
		_worldThread->join();
	_worldThread.reset();
}

void GameInstance::GameLoop()
{
	DynamicFpsController fpscontroller(144);
	fpscontroller.reset();

	// 主循环
	while (!_stop)
	{
		float dt = fpscontroller.getTimeDiffMS();

		_world->update(dt);
		if (_stop)
			break;
		BroadCaseGameState();

		fpscontroller.run();
	}
}

void GameInstance::End()
{
}

bool GameInstance::AddPlayer(std::shared_ptr<GamePlayer> player)
{
	auto gaurd = _PlayerIdToGamePlayer.MakeLockGuard();
	if (_PlayerIdToGamePlayer.Exist(player->playerid))
		return false;

	_PlayerIdToGamePlayer[player->playerid] = player;
	return true;
}

bool GameInstance::RemovePlayer(const PlayerID& playerId)
{
	auto gaurd = _PlayerIdToGamePlayer.MakeLockGuard();
	if (!_PlayerIdToGamePlayer.Exist(playerId))
		return false;

	_PlayerIdToGamePlayer.Erase(playerId);
	return true;
}

std::shared_ptr<GamePlayer> GameInstance::GetPlayer(const PlayerID& playerId)
{
	auto gaurd = _PlayerIdToGamePlayer.MakeLockGuard();
	if (!_PlayerIdToGamePlayer.Exist(playerId))
		return nullptr;

	return _PlayerIdToGamePlayer[playerId];
}

std::vector<PlayerID> GameInstance::GetAllPlayerIds() const
{
	return _PlayerIdToGamePlayer.GetKeys();
}

void GameInstance::ProcessPlayerInput(const PlayerID& playerId, const json& input)
{
	if (auto* inputsystem = _world->getSystem<ServerInputSystem>())
	{

		if (!input.contains("inputstate") || !input["inputstate"].is_object())
			return;

		json js_inputstate = input["inputstate"];

		InputState newstate;
		newstate.forward = js_inputstate.value("forward", 0) > 0;
		newstate.backword = js_inputstate.value("backword", 0) > 0;
		newstate.left = js_inputstate.value("left", 0) > 0;
		newstate.right = js_inputstate.value("right", 0) > 0;
		newstate.fire = js_inputstate.value("fire", 0) > 0;

		inputsystem->InputNewState(playerId, newstate);
	}
}

json GameInstance::GetGameState() const
{
	return json();
}

json GameInstance::GetPlayerView(const PlayerID& playerId) const
{
	return json();
}

void GameInstance::GameOver()
{
	_state = State::ENDED;
	_stop = true;

	json js_GameOver;
	js_GameOver["command"] = GameService_GameOverInfo;
	js_GameOver["gameid"] = _gameId;
	js_GameOver["winnerid"] = "";

	if (_sender)
		_sender->Broadcast(js_GameOver);
	if (_service)
		_service->GameOver(_gameId);
}

void GameInstance::GameOverWithWinner(const PlayerID& winner)
{
	_state = State::ENDED;
	_stop = true;

	json js_GameOver;
	js_GameOver["command"] = GameService_GameOverInfo;
	js_GameOver["gameid"] = _gameId;
	js_GameOver["winnerid"] = winner;

	if (_sender)
		_sender->Broadcast(js_GameOver);
	if (_service)
		_service->GameOver(_gameId);
}

void GameInstance::PlayerEliminated(const PlayerID& playerId, const PlayerID& killer)
{
	std::string killerDescription;
	if (killer.empty())
	{
	}
	else if (killer == "AI")
	{
		killerDescription = "人机";
	}
	else
	{
		if (_PlayerIdToGamePlayer.Exist(killer))
			killerDescription = _PlayerIdToGamePlayer[killer]->name;
	}
	if (_sender)
	{
		json js_Eliminate;
		js_Eliminate["command"] = GameService_EliminateInfo;
		js_Eliminate["gameid"] = _gameId;
		js_Eliminate["playerid"] = playerId;
		js_Eliminate["killerDescription"] = killerDescription;
		_sender->SendToPlayer(playerId, js_Eliminate);
	}
}
