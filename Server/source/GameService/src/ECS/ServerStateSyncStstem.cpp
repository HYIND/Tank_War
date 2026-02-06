#include "ECS/Systems/ServerStateSyncStstem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"

ServerStateSyncStstem::ServerStateSyncStstem()
{
	_gamestate_tripbuffer = std::make_shared<TripleBuffer<GameState>>();
	for (int i = 0; i < 3; i++)
		_gamestate_tripbuffer->setInitialValue(i, GameState());
}

ServerStateSyncStstem::~ServerStateSyncStstem()
{
	SetSyncFps(60.f);
}

void ServerStateSyncStstem::postUpdate(float dt)
{
	_deltatime_Accumulator += dt;
	if (_deltatime_Accumulator < _update_deltatime_ms)
		return;

	_deltatime_Accumulator -= _update_deltatime_ms;

	GameState allstate;

	auto &world = getWorld();
	std::vector<Entity> entities = world.getEntitiesWith<TagSync>();
	for (auto &entity : entities)
	{
		auto &sync = world.getComponent<TagSync>(entity);

		if (sync.syncEntityType == TagSync::SyncEntityType::TANK)
		{
			if (!entity.hasComponents<TagSync, TagTank, TankProperty, Transform, Movement, Health>())
				continue;
			handleSyncTank(allstate, entity);
		}
		else if (sync.syncEntityType == TagSync::SyncEntityType::BULLET)
		{
			if (!entity.hasComponents<TagSync, TagBullet, BulletProperty, Transform, Movement>())
				continue;
			handleSyncBullet(allstate, entity);
		}
		else if (sync.syncEntityType == TagSync::SyncEntityType::WALL)
		{
			if (!entity.hasComponents<TagSync, TagWall, WallProperty, Transform, Health>())
				continue;
			handleSyncWall(allstate, entity);
		}
		else if (sync.syncEntityType == TagSync::SyncEntityType::PROP)
		{
			if (!entity.hasComponents<TagSync, TagProp, PropProperty, Transform>())
				continue;
			handleSyncProp(allstate, entity);
		}
	}

	allstate.updateTime = Tool::GetTimestampMilliseconds();
	auto &write = _gamestate_tripbuffer->acquireWriteBuffer();
	write = allstate;
	_gamestate_tripbuffer->submitWriteBuffer();
}

GameState &ServerStateSyncStstem::GetGameState()
{
	return _gamestate_tripbuffer->acquireReadBuffer();
}

void ServerStateSyncStstem::SetSyncFps(float fps)
{
	_update_fps = fps;
	_update_deltatime_ms = 1000.f / fps;
	_deltatime_Accumulator = 0.f;
}

float ServerStateSyncStstem::GetSyncFps()
{
	return _update_fps;
}

void ServerStateSyncStstem::handleSyncTank(GameState &allstate, Entity entity)
{
	auto &world = getWorld();

	auto &sync = world.getComponent<TagSync>(entity);
	auto &tankProperty = world.getComponent<TankProperty>(entity);
	auto &trans = world.getComponent<Transform>(entity);
	auto &movement = world.getComponent<Movement>(entity);
	auto &health = world.getComponent<Health>(entity);

	TankState state;
	state.entityId = entity.getId();
	state.syncId = sync.syncId;
	state.owner = tankProperty.owner;
	state.playerId = tankProperty.playerId;
	state.posX = trans.position.x;
	state.posY = trans.position.y;
	state.rotation = trans.rotation;
	state.width = tankProperty.width;
	state.height = tankProperty.height;
	state.maxHealth = health.maxHealth;
	state.currentHealth = health.currentHealth;
	state.maxMoveSpeed = movement.maxMoveSpeed;
	state.maxRotationSpeed = movement.maxRotationSpeed;
	state.currentMoveSpeed = movement.currentMoveSpeed;
	state.currentRotationSpeed = movement.currentRotationSpeed;
	state.isAlive = health.isAlive();
	state.isMoving = movement.currentMoveSpeed != 0 || movement.currentRotationSpeed != 0;

	allstate.tankState.emplace_back(state);
}

void ServerStateSyncStstem::handleSyncBullet(GameState &allstate, Entity entity)
{
	auto &world = getWorld();

	auto &sync = world.getComponent<TagSync>(entity);
	auto &bulletProperty = world.getComponent<BulletProperty>(entity);
	auto &trans = world.getComponent<Transform>(entity);
	auto &movement = world.getComponent<Movement>(entity);

	BulletState state;
	state.entityId = entity.getId();
	state.syncId = sync.syncId;
	state.ownerId = bulletProperty.owner.getId();
	state.ownerPlayerId = bulletProperty.ownerPlayerId;
	state.posX = trans.position.x;
	state.posY = trans.position.y;
	state.rotation = trans.rotation;
	state.radius = bulletProperty.radius;
	state.bulletDamage = bulletProperty.bulletDamage;
	state.maxMoveSpeed = movement.maxMoveSpeed;
	state.maxRotationSpeed = movement.maxRotationSpeed;
	state.currentMoveSpeed = movement.currentMoveSpeed;
	state.currentRotationSpeed = movement.currentRotationSpeed;

	allstate.bulletState.emplace_back(state);
}

void ServerStateSyncStstem::handleSyncProp(GameState &allstate, Entity entity)
{
	auto &world = getWorld();

	auto &sync = world.getComponent<TagSync>(entity);
	auto &propProperty = world.getComponent<PropProperty>(entity);
	auto &trans = world.getComponent<Transform>(entity);

	PropState state;
	state.entityId = entity.getId();
	state.syncId = sync.syncId;
	state.type = propProperty.type;
	state.posX = trans.position.x;
	state.posY = trans.position.y;
	state.rotation = trans.rotation;
	state.width = propProperty.width;
	state.height = propProperty.height;
	state.duration = propProperty.duration;

	if (auto life = entity.tryGetComponent<LifeTime>())
		state.lifetime = life->remainingTime;
	else
		state.lifetime = 0;

	allstate.propState.emplace_back(state);
}

void ServerStateSyncStstem::handleSyncWall(GameState &allstate, Entity entity)
{
	auto &world = getWorld();

	auto &sync = world.getComponent<TagSync>(entity);
	auto &wallproperty = world.getComponent<WallProperty>(entity);
	auto &trans = world.getComponent<Transform>(entity);
	auto &health = world.getComponent<Health>(entity);

	WallState state;
	state.entityId = entity.getId();
	state.syncId = sync.syncId;
	state.type = wallproperty.type;
	state.posX = trans.position.x;
	state.posY = trans.position.y;
	state.rotation = trans.rotation;
	state.width = wallproperty.width;
	state.height = wallproperty.height;
	state.currenthealth = health.currentHealth;
	state.maxhealth = health.maxHealth;

	allstate.wallState.emplace_back(state);
}
