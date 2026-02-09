#include "Manager/ConnectManager.h"
#include "Manager/GameWorldManager.h"
#include "Manager/UserInfoManager.h"

#include "Helper/DynamicFpsController.h"

#include "ECS/Systems/AllSystem.h"

#include "ECS/Components/AllComponent.h"

#include "ECS/Factory/TankFactory.h"
#include "ECS/Factory/WallFactory.h"
#include "ECS/Factory/PropFactory.h"
#include "ECS/Factory/BulletFactory.h"

#include "Scene.h"

void LoadLocalGameMapInfoToWorld(std::shared_ptr<World> world, const MapInfo& mapinfo)
{
	// 加载背景（如果有）
	if (!mapinfo.backGrounp_resname.empty())
	{
		Entity map_entity = world->createEntity();
		Pos2 map_center((MapBoundary::left + MapBoundary::right) / 2.f, (MapBoundary::top + MapBoundary::bottom) / 2.f);
		Vec2 map_size(MapBoundary::right - MapBoundary::left, MapBoundary::bottom - MapBoundary::top);
		map_entity.addComponent<Transform>(map_center, 0.f, Vec2(1, 1));
		auto& map_sprite = map_entity.addComponent<Sprite>(map_size.x, map_size.y, ResFactory->GetBitMapRes(mapinfo.backGrounp_resname));
		map_sprite.layer = (int)RenderLayer::MapLayer;
	}

	static std::vector<TankProperty::TankOwner> avaliableplayer = { TankProperty::TankOwner::PLAYER1,TankProperty::TankOwner::PLAYER2 };

	// 生成玩家坦克
	for (int i = 0; i < mapinfo.tankbirthinfos.size() && i < avaliableplayer.size(); i++)
	{
		auto& tankInfo = mapinfo.tankbirthinfos[i];

		// 使用TankFactory创建玩家坦克
		Entity tank = TankFactory::CreateLocalGameTank(
			*world,
			avaliableplayer[i],
			tankInfo.position.x,
			tankInfo.position.y,
			tankInfo.width,
			tankInfo.height
		);

		// 设置旋转和视觉状态
		if (auto* trans = tank.tryGetComponent<Transform>())
			trans->rotation = tankInfo.rotation;

		if (auto* visual = tank.tryGetComponent<TankVisual>())
			visual->visualstate = tankInfo.visual;
	}

	// 生成AI坦克
	for (auto& aiTankInfo : mapinfo.aitankbirthinfos)
	{
		Entity aiTank = TankFactory::CreateLocalGameTank(
			*world,
			TankProperty::TankOwner::AI,
			aiTankInfo.position.x,
			aiTankInfo.position.y,
			aiTankInfo.width,
			aiTankInfo.height
		);

		if (auto* trans = aiTank.tryGetComponent<Transform>())
			trans->rotation = aiTankInfo.rotation;

		if (auto* visual = aiTank.tryGetComponent<TankVisual>())
			visual->visualstate = aiTankInfo.visual;
	}

	// 生成道具
	for (auto& propInfo : mapinfo.propbirthinfos)
	{
		Entity prop = PropFactory::CreateProp(
			*world,
			propInfo.type,
			propInfo.duration,
			propInfo.position.x,
			propInfo.position.y,
			propInfo.width,
			propInfo.height
		);

		if (auto* trans = prop.tryGetComponent<Transform>())
			trans->rotation = propInfo.rotation;
	}

	for (auto& wallInfo : mapinfo.wallbirthinfos)
	{
		Entity wall = WallFactory::CreateWall(
			*world,
			wallInfo.type,
			wallInfo.position.x,
			wallInfo.position.y,
			wallInfo.width,
			wallInfo.height
		);

		if (auto* trans = wall.tryGetComponent<Transform>())
			trans->rotation = wallInfo.rotation;

		if (auto* health = wall.tryGetComponent<Health>())
		{
			health->maxHealth = wallInfo.health;
			health->currentHealth = wallInfo.health;
		}
	}
}

GameWorldManager* GameWorldManager::Instance()
{
	static GameWorldManager* m_instance = new GameWorldManager();
	return m_instance;
}

void GameWorldManager::SyncFromServerState(const json& js)
{
	if (!_world)
		return;

	auto& world = _world;

	if (auto sync = world->getSystem<ClientStateSyncStstem>())
	{
		if (js.contains("gamestate") && js["gamestate"].is_object())
		{
			GameState gamestate = GameState::fromJson(js["gamestate"]);
			sync->InputGameState(gamestate);
		}
	}
}

void GameWorldManager::ProcessEliminateInfo(const json& js)
{
	if (!js.contains("gameid") || !js["gameid"].is_string())
		return;
	if (!js.contains("playerid") || !js["playerid"].is_string())
		return;
	if (!js.contains("killerDescription") || !js["killerDescription"].is_string())
		return;

	GameID gameid = js.value("gameid", "");
	PlayerID playerid = js.value("playerid", "");
	std::string killerDescription = js.value("killerDescription", "");

	if (gameid == UserInfoManager::Instance()->gameid() && playerid == UserInfoManager::Instance()->usertoken())
	{
		std::wstring formatstring;
		if (killerDescription.empty())
		{
			formatstring = std::format(L"您已经被淘汰！");
		}
		else
		{
			formatstring = std::format(L"您已经被{}淘汰！", Tool::UTF8ToWString(killerDescription));
		}
		//CreateTooltip(_hwnd, NULL, formatstring.c_str());
	}
}

void GameWorldManager::ProcessGameOver(const json& js)
{
	StopWorld();
	if (!js.contains("gameid") || !js["gameid"].is_string())
		return;
	if (!js.contains("winnerid") || !js["winnerid"].is_string())
		return;

	GameID gameid = js.value("gameid", "");
	PlayerID winnerid = js.value("winnerid", "");


	if (gameid == UserInfoManager::Instance()->gameid())
	{
		if (UserInfoManager::Instance()->isMyToken(winnerid))
		{
			PostMessage(_hwnd, WM_COMMAND, WIN, (LPARAM)_hwnd);
		}
		else
		{
			PostMessage(_hwnd, WM_COMMAND, FAIL, (LPARAM)_hwnd);
		}
	}
}

GameWorldManager::GameWorldManager()
	:_stop(true)
{
}

void GameWorldManager::InitGameWorld(GameMode mode, MapID mapid)
{
	_world = std::make_shared<World>();

	if (mode == GameMode::RunGame)
	{
		// 通过World注册系统
		auto& inputSystem = _world->registerSystem<LocalInputSystem>();
		auto& velocityControlSystem = _world->registerSystem<VelocityControlSystem>();
		auto& movementSystem = _world->registerSystem<MovementSystem>();
		auto& weaponSystem = _world->registerSystem<WeaponSystem>();
		auto& bulletSystem = _world->registerSystem<BulletSystem>();
		auto& renderSystem = _world->registerSystem<RenderSystem>();
		auto& lifetimeSystem = _world->registerSystem<LifetimeSystem>();
		auto& physicsSystem = _world->registerSystem<PhysicsSystem>();
		auto& destroySystem = _world->registerSystem<DestroySystem>();
		auto& healthSystem = _world->registerSystem<HealthSystem>();
		auto& effectSystem = _world->registerSystem<EffectSystem>();
		auto& wallSystem = _world->registerSystem<WallSystem>();
		auto& propSystem = _world->registerSystem<PropSystem>();
		auto& tankSystem = _world->registerSystem<TankSystem>();


		lifetimeSystem.setPriority(10000);
		inputSystem.setPriority(1000);
		velocityControlSystem.setPriority(500);
		movementSystem.setPriority(500);
		physicsSystem.setPriority(300);
		destroySystem.setPriority(-9000);
		renderSystem.setPriority(-10000);

		auto mapinfo = MapManager::Instance()->getMap(mapid);
		LoadLocalGameMapInfoToWorld(_world, mapinfo);

		Entity map_boundary = _world->createEntityWithTag<TagGameBoundary>();
		auto& boundary = map_boundary.addComponent<BoundaryPhysisc>(MapBoundary::left, MapBoundary::top, MapBoundary::right, MapBoundary::bottom);
	}
	else if (mode == GameMode::MapEdit)
	{
	}
}

void GameWorldManager::InitOnlineGameWorld()
{
	_world = std::make_shared<World>();


	// 通过World注册系统
	auto& inputSystem = _world->registerSystem<ClientInputSystem>();
	auto& velocityControlSystem = _world->registerSystem<VelocityControlSystem>();
	//auto& movementSystem = _world->registerSystem<MovementSystem>();
	//auto& weaponSystem = _world->registerSystem<WeaponSystem>();
	auto& bulletSystem = _world->registerSystem<BulletSystem>();
	auto& renderSystem = _world->registerSystem<RenderSystem>();
	auto& lifetimeSystem = _world->registerSystem<LifetimeSystem>();
	auto& predictSystem = _world->registerSystem<PredictionSystem>();
	auto& healthSystem = _world->registerSystem<HealthSystem>();
	auto& effectSystem = _world->registerSystem<EffectSystem>();
	auto& wallSystem = _world->registerSystem<WallSystem>();
	auto& propSystem = _world->registerSystem<PropSystem>();
	auto& tankSystem = _world->registerSystem<TankSystem>();
	auto& syncSystem = _world->registerSystem<ClientStateSyncStstem>();
	auto& interpolationSystem = _world->registerSystem<InterpolationSystem>();


	syncSystem.setPriority(20000);
	lifetimeSystem.setPriority(10000);
	inputSystem.setPriority(1000);
	velocityControlSystem.setPriority(500);
	//movementSystem.setPriority(500);
	interpolationSystem.setPriority(400);
	predictSystem.setPriority(300);
	renderSystem.setPriority(-10000);

	//auto mapinfo = MapManager::Instance()->getMap(0);
	LoadLocalGameMapInfoToWorld(_world, MapInfo{ .backGrounp_resname = ResName::sandBK });

	Entity map_boundary = _world->createEntityWithTag<TagGameBoundary>();
	auto& boundary = map_boundary.addComponent<BoundaryPhysisc>(MapBoundary::left, MapBoundary::top, MapBoundary::right, MapBoundary::bottom);
}

std::shared_ptr<World> GameWorldManager::GetGameWorld()
{
	return _world;
}


void GameWorldManager::RunWorld()
{
	if (!_world)
		return;


	// 启动世界
	_world->setLogicDeltaTime(1000.f / 165.f);
	_world->setFixedDeltaTime(1000.f / 60.f);
	_world->start();
	_stop = false;
	_worldThread = std::make_shared<std::thread>(&GameWorldManager::WorldLoop, this);
}

void GameWorldManager::WorldLoop()
{
	int targetfps = (1000.f / std::min(_world->getFixedDeltaTime(), _world->getLogicDeltaTime())) + 1;
	DynamicFpsController fpscontroller(targetfps);
	fpscontroller.reset();

	// 主循环
	while (!_stop)
	{
		float dt = fpscontroller.getTimeDiffMS();

		_world->update(dt);

		fpscontroller.run();
	}
}

void GameWorldManager::StopWorld()
{
	_stop = true;
	if (_worldThread)
	{
		if (_worldThread->joinable())
			_worldThread->join();
		_worldThread.reset();
	}
	_world.reset();
}

std::shared_ptr<std::thread> GameWorldManager::GetWorldThread()
{
	return _worldThread;
}


