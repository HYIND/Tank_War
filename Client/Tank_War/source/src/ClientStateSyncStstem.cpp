#include "ECS/Systems/ClientStateSyncStstem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"
#include "ECS/Factory/TankFactory.h"
#include "ECS/Factory/BulletFactory.h"
#include "ECS/Factory/PropFactory.h"
#include "ECS/Factory/WallFactory.h"
#include "Manager/UserInfoManager.h"
#include "ECS/Systems/InterpolationSystem.h"
#include <unordered_set>

ClientStateSyncStstem::ClientStateSyncStstem()
{
	_gamestate_tripbuffer = std::make_shared<TripleBuffer<GameState>>();
	for (int i = 0; i < 3; i++)
		_gamestate_tripbuffer->setInitialValue(i, GameState());
}

ClientStateSyncStstem::~ClientStateSyncStstem()
{
}

void ClientStateSyncStstem::preUpdate(float dt)
{
	GameState& allstate = _gamestate_tripbuffer->acquireReadBuffer();
	if (allstate.hasConsume)
		return;

	allstate.hasConsume = true;

	auto& world = getWorld();

	std::unordered_map<SyncID, Entity> clientTankMap;
	std::unordered_map<SyncID, Entity> clientBulletMap;
	std::unordered_map<SyncID, Entity> clientWallMap;
	std::unordered_map<SyncID, Entity> clientPropMap;

	std::vector<Entity> entities = world.getEntitiesWith<TagSync>();
	for (auto& entity : entities)
	{
		if (!entity.hasComponents<TagSync>())
			continue;

		auto& sync = world.getComponent<TagSync>(entity);
		if (sync.syncEntityType == TagSync::SyncEntityType::TANK)
		{
			clientTankMap[sync.syncId] = entity;
		}
		else if (sync.syncEntityType == TagSync::SyncEntityType::BULLET)
		{
			clientBulletMap[sync.syncId] = entity;
		}
		else if (sync.syncEntityType == TagSync::SyncEntityType::WALL)
		{
			clientWallMap[sync.syncId] = entity;
		}
		else if (sync.syncEntityType == TagSync::SyncEntityType::PROP)
		{
			clientPropMap[sync.syncId] = entity;
		}
	}

	SyncTanks(allstate.updateTime, allstate.tankState, clientTankMap);
	SyncBullet(allstate.updateTime, allstate.bulletState, clientBulletMap);
	SyncWall(allstate.wallState, clientWallMap);
	SyncProp(allstate.propState, clientPropMap);
}

void ClientStateSyncStstem::InputGameState(const GameState& newstate)
{
	auto& write = _gamestate_tripbuffer->acquireWriteBuffer();
	write = newstate;
	_gamestate_tripbuffer->submitWriteBuffer();
}

void ClientStateSyncStstem::SyncTanks(uint64_t server_timestamp, std::vector<TankState>& tankStates, std::unordered_map<SyncID, Entity>& clienttankmap)
{
	auto& world = getWorld();

	// 第三步：构建服务器实体ID集合
	std::unordered_set<SyncID> serverEntity;
	for (auto& tankstate : tankStates)
	{
		serverEntity.insert(tankstate.syncId);
	}

	// 第四步：销毁客户端多余实体
	for (auto& [syncId, entity] : clienttankmap)
	{
		if (serverEntity.find(syncId) == serverEntity.end())
		{
			// 服务器没有这个实体，但在客户端存在，需要销毁
			if (auto trans = entity.tryGetComponent<Transform>())
			{

				TankDestroyedEvent event;
				event.tank = entity;
				event.position = trans->position;
				world.destroyEntityLaterWithEvent<TankDestroyedEvent>(entity, event);
			}
			else
			{
				world.destroyEntityLater(entity);
			}
		}
	}

	// 第五步：同步/创建服务器实体
	for (auto& tankstate : tankStates)
	{
		auto it = clienttankmap.find(tankstate.syncId);
		if (it != clienttankmap.end())
		{
			Entity& entity = clienttankmap[tankstate.syncId];
			auto& sync = world.getComponent<TagSync>(entity);
			if (sync.syncEntityType != TagSync::SyncEntityType::TANK)
				continue;
			if (!entity.hasComponents<TagSync, TagTank, TankProperty, Transform, Movement, Health>())
				continue;
			handleSyncTankFromServer(server_timestamp, tankstate, entity);
		}
		else
		{
			handleCreateClientTank(server_timestamp, tankstate);
		}
	}
}

void ClientStateSyncStstem::SyncBullet(uint64_t server_timestamp, std::vector<BulletState>& bulletState, std::unordered_map<SyncID, Entity>& clientbulletmap)
{
	auto& world = getWorld();

	// 第三步：构建服务器实体ID集合
	std::unordered_set<SyncID> serverEntity;
	for (auto& bulletstate : bulletState)
	{
		serverEntity.insert(bulletstate.syncId);
	}

	// 第四步：销毁客户端多余实体
	for (auto& [syncId, entity] : clientbulletmap)
	{
		if (serverEntity.find(syncId) == serverEntity.end())
		{
			// 服务器没有这个实体，但在客户端存在，需要销毁
			world.destroyEntity(entity);
		}
	}

	// 第五步：同步/创建服务器实体
	for (auto& bulletstate : bulletState)
	{
		auto it = clientbulletmap.find(bulletstate.syncId);
		if (it != clientbulletmap.end())
		{
			Entity& entity = clientbulletmap[bulletstate.syncId];
			auto& sync = world.getComponent<TagSync>(entity);
			if (sync.syncEntityType != TagSync::SyncEntityType::BULLET)
				continue;
			if (!entity.hasComponents<TagSync, TagBullet, BulletProperty, Transform, Movement >())
				continue;
			handleSyncBulletFromServer(server_timestamp, bulletstate, entity);
		}
		else
		{
			handleCreateClientBullet(server_timestamp, bulletstate);
		}
	}
}

void ClientStateSyncStstem::SyncWall(std::vector<WallState>& wallStates, std::unordered_map<SyncID, Entity>& clientwallmap)
{
	auto& world = getWorld();

	// 第三步：构建服务器实体ID集合
	std::unordered_set<SyncID> serverEntity;
	for (auto& wallstate : wallStates)
	{
		serverEntity.insert(wallstate.syncId);
	}

	// 第四步：销毁客户端多余实体
	for (auto& [syncId, entity] : clientwallmap)
	{
		if (serverEntity.find(syncId) == serverEntity.end())
		{
			// 服务器没有这个实体，但在客户端存在，需要销毁
			world.destroyEntity(entity);
		}
	}

	// 第五步：同步/创建服务器实体
	for (auto& walltstate : wallStates)
	{
		auto it = clientwallmap.find(walltstate.syncId);
		if (it != clientwallmap.end())
		{
			Entity& entity = clientwallmap[walltstate.syncId];
			auto& sync = world.getComponent<TagSync>(entity);
			if (sync.syncEntityType != TagSync::SyncEntityType::WALL)
				continue;
			if (!entity.hasComponents<TagSync, TagWall, WallProperty, Transform, Health>())
				continue;
			handleSyncWallFromServer(walltstate, entity);
		}
		else
		{
			handleCreateClientWall(walltstate);
		}
	}
}

void ClientStateSyncStstem::SyncProp(std::vector<PropState>& propStates, std::unordered_map<SyncID, Entity>& clientpropmap)
{
	auto& world = getWorld();

	// 第三步：构建服务器实体ID集合
	std::unordered_set<SyncID> serverEntity;
	for (auto& propstate : propStates)
	{
		serverEntity.insert(propstate.syncId);
	}

	// 第四步：销毁客户端多余实体
	for (auto& [syncId, entity] : clientpropmap)
	{
		if (serverEntity.find(syncId) == serverEntity.end())
		{
			// 服务器没有这个实体，但在客户端存在，需要销毁
			world.destroyEntity(entity);
		}
	}

	// 第五步：同步/创建服务器实体
	for (auto& propstate : propStates)
	{
		auto it = clientpropmap.find(propstate.syncId);
		if (it != clientpropmap.end())
		{
			Entity& entity = clientpropmap[propstate.syncId];
			auto& sync = world.getComponent<TagSync>(entity);
			if (sync.syncEntityType != TagSync::SyncEntityType::PROP)
				continue;
			if (!entity.hasComponents<TagSync, TagProp, PropProperty, Transform>())
				continue;
			handleSyncPropFromServer(propstate, entity);
		}
		else
		{
			handleCreateClientProp(propstate);
		}
	}
}

void ClientStateSyncStstem::handleSyncTankFromServer(uint64_t server_timestamp, TankState& state, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& tankProperty = world.getComponent<TankProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);
	auto& movement = world.getComponent<Movement>(entity);
	auto& health = world.getComponent<Health>(entity);

	tankProperty.playerId = state.playerId;
	tankProperty.owner = state.owner;

	if (state.playerId != UserInfoManager::Instance()->usertoken() && entity.hasComponent<Interpolation>() && world.getSystem<InterpolationSystem>() != nullptr)
	{
		auto& inter = entity.getComponent<Interpolation>();
		Interpolation::Snapshot snap;
		snap.position = { state.posX,state.posY };
		snap.rotation = state.rotation;

		Vec2 direction = { cos(Tool::AngleToRadian(snap.rotation)),sin(Tool::AngleToRadian(snap.rotation)) };
		Vec2 velocity(
			direction.x * state.currentMoveSpeed,
			direction.y * state.currentMoveSpeed
		);
		snap.velocity = velocity;
		snap.angularVelocity = state.currentRotationSpeed;
		snap.timestamp = server_timestamp;
		snap.receiveTime = Tool::GetTimestampMilliseconds();
		inter.snapshots.push_back(std::move(snap));
	}
	else
	{
		trans.position.x = state.posX;
		trans.position.y = state.posY;
		trans.rotation = state.rotation;
	}


	tankProperty.width = state.width;
	tankProperty.height = state.height;

	health.maxHealth = state.maxHealth;
	health.currentHealth = state.currentHealth;

	movement.maxMoveSpeed = state.maxMoveSpeed;
	movement.maxRotationSpeed = state.maxRotationSpeed;
	movement.currentMoveSpeed = state.currentMoveSpeed;
	movement.currentRotationSpeed = state.currentRotationSpeed;
}

void ClientStateSyncStstem::handleCreateClientTank(uint64_t server_timestamp, TankState& state)
{
	auto& world = getWorld();

	auto entity = TankFactory::CreateClientTank(world,
		state.owner, state.syncId, state.playerId,
		state.posX, state.posY, state.width, state.height,
		state.rotation
	);

	if (auto move = entity.tryGetComponent<Movement>())
	{
		move->maxMoveSpeed = state.maxMoveSpeed;
		move->maxRotationSpeed = state.maxRotationSpeed;
		move->currentMoveSpeed = state.currentMoveSpeed;
		move->currentRotationSpeed = state.currentRotationSpeed;
	}

	if (auto health = entity.tryGetComponent<Health>())
	{
		health->maxHealth = state.maxHealth;
		health->currentHealth = state.currentHealth;
	}

	if (state.playerId != UserInfoManager::Instance()->usertoken() && entity.hasComponent<Interpolation>() && world.getSystem<InterpolationSystem>() != nullptr)
	{
		auto& inter = entity.getComponent<Interpolation>();
		Interpolation::Snapshot snap;
		snap.position = { state.posX,state.posY };
		snap.rotation = state.rotation;

		Vec2 direction = { cos(Tool::AngleToRadian(snap.rotation)),sin(Tool::AngleToRadian(snap.rotation)) };
		Vec2 velocity(
			direction.x * state.currentMoveSpeed,
			direction.y * state.currentMoveSpeed
		);
		snap.velocity = velocity;
		snap.angularVelocity = state.currentRotationSpeed;
		snap.timestamp = server_timestamp;
		snap.receiveTime = Tool::GetTimestampMilliseconds();
		inter.snapshots.push_back(std::move(snap));
	}
}

void ClientStateSyncStstem::handleSyncBulletFromServer(uint64_t server_timestamp, BulletState& state, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& bulletProperty = world.getComponent<BulletProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);
	auto& movement = world.getComponent<Movement>(entity);

	bulletProperty.ownerPlayerId = state.ownerPlayerId;
	//bulletProperty.owner = state.owner;

	if (entity.hasComponent<Interpolation>() && world.getSystem<InterpolationSystem>() != nullptr)
	{
		auto& inter = entity.getComponent<Interpolation>();
		Interpolation::Snapshot snap;
		snap.position = { state.posX,state.posY };
		snap.rotation = state.rotation;

		Vec2 direction = { cos(Tool::AngleToRadian(snap.rotation)),sin(Tool::AngleToRadian(snap.rotation)) };
		Vec2 velocity(
			direction.x * state.currentMoveSpeed,
			direction.y * state.currentMoveSpeed
		);
		snap.velocity = velocity;
		snap.angularVelocity = state.currentRotationSpeed;
		snap.timestamp = server_timestamp;
		snap.receiveTime = Tool::GetTimestampMilliseconds();
		inter.snapshots.push_back(std::move(snap));
	}
	else
	{
		trans.position.x = state.posX;
		trans.position.y = state.posY;
		trans.rotation = state.rotation;
	}

	bulletProperty.radius = state.radius;

	movement.maxMoveSpeed = state.maxMoveSpeed;
	movement.maxRotationSpeed = state.maxRotationSpeed;
	movement.currentMoveSpeed = state.currentMoveSpeed;
	movement.currentRotationSpeed = state.currentRotationSpeed;
}

void ClientStateSyncStstem::handleCreateClientBullet(uint64_t server_timestamp, BulletState& state)
{
	auto& world = getWorld();

	Entity owner;
	std::vector<Entity> entities = world.getEntitiesWith<TagTank, TankProperty>();
	for (auto& entity : entities)
	{
		if (auto trankproperty = entity.tryGetComponent<TankProperty>())
		{
			if (trankproperty->playerId == state.ownerPlayerId)
			{
				owner = entity;
				break;
			}
		}
	}

	auto entity = BulletFactory::CreateClientBullet(world,
		state.syncId, owner, state.ownerPlayerId,
		state.bulletDamage, state.currentMoveSpeed,
		state.posX, state.posY, state.radius,
		state.rotation
	);

	if (auto move = entity.tryGetComponent<Movement>())
	{
		move->maxMoveSpeed = state.maxMoveSpeed;
		move->maxRotationSpeed = state.maxRotationSpeed;
		move->currentMoveSpeed = state.currentMoveSpeed;
		move->currentRotationSpeed = state.currentRotationSpeed;
	}

	if (entity.hasComponent<Interpolation>() && world.getSystem<InterpolationSystem>() != nullptr && world.getSystem<InterpolationSystem>() != nullptr)
	{
		auto& inter = entity.getComponent<Interpolation>();
		Interpolation::Snapshot snap;
		snap.position = { state.posX,state.posY };
		snap.rotation = state.rotation;

		Vec2 direction = { cos(Tool::AngleToRadian(snap.rotation)),sin(Tool::AngleToRadian(snap.rotation)) };
		Vec2 velocity(
			direction.x * state.currentMoveSpeed,
			direction.y * state.currentMoveSpeed
		);
		snap.velocity = velocity;
		snap.angularVelocity = state.currentRotationSpeed;
		snap.timestamp = server_timestamp;
		snap.receiveTime = Tool::GetTimestampMilliseconds();
		inter.snapshots.push_back(std::move(snap));
	}
}

void ClientStateSyncStstem::handleSyncWallFromServer(WallState& state, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& wallProperty = world.getComponent<WallProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);
	auto& health = world.getComponent<Health>(entity);

	trans.position.x = state.posX;
	trans.position.y = state.posY;
	trans.rotation = state.rotation;

	wallProperty.type = state.type;
	wallProperty.width = state.width;
	wallProperty.height = state.height;

	health.currentHealth = state.currenthealth;
	health.maxHealth = state.maxhealth;
}

void ClientStateSyncStstem::handleCreateClientWall(WallState& state)
{
	auto& world = getWorld();

	auto entity = WallFactory::CreateClientWall(world,
		state.syncId, state.type,
		state.posX, state.posY, state.rotation,
		state.width, state.height,
		state.currenthealth, state.maxhealth
	);
}

void ClientStateSyncStstem::handleSyncPropFromServer(PropState& state, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& propProperty = world.getComponent<PropProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);

	trans.position.x = state.posX;
	trans.position.y = state.posY;
	trans.rotation = state.rotation;

	propProperty.type = state.type;
	propProperty.width = state.width;
	propProperty.height = state.height;
	propProperty.duration = state.duration;

	if (state.lifetime > 0.f)
	{
		if (auto life = entity.tryGetComponent<LifeTime>())
		{
			life->remainingTime = state.lifetime;
		}
	}
}

void ClientStateSyncStstem::handleCreateClientProp(PropState& state)
{
	auto& world = getWorld();

	auto entity = PropFactory::CreateClientProp(world,
		state.syncId, state.type, state.duration,
		state.posX, state.posY, state.rotation,
		state.width, state.height,
		state.lifetime
	);
}
