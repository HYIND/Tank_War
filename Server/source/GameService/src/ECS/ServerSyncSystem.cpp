#include "ECS/Systems/ServerSyncSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"
#include "command.h"

ServerSyncSystem::ServerSyncSystem()
{
	_gamestate_tripbuffer = std::make_shared<TripleBuffer<GameState>>();
	for (int i = 0; i < 3; i++)
		_gamestate_tripbuffer->setInitialValue(i, GameState());
}

ServerSyncSystem::~ServerSyncSystem()
{
	SetSyncFps(60.f);
}

void ServerSyncSystem::onAttach(World& world)
{
	world.Subscribe<ServerSyncSystem, TankDestroyedEvent>(
		[this](const TankDestroyedEvent& event) {
			if (event.tank.hasComponent<TagSync>())
			{
				auto data = std::make_shared<TankDestroyedEventData>();
				data->tankId = event.tank.getComponent<TagSync>().syncId;
				if (event.killer && event.killer->hasComponent<TagSync>())
					data->killerId = event.killer->getComponent<TagSync>().syncId;
				data->position = event.position;

				SyncEvent syncEvent(data->getType(), data, Tool::GetTimestampMilliseconds(), m_world ? m_world->getFrameCount() : 0);
				BroadCaseEvent(syncEvent);
			}
		});

	world.Subscribe<ServerSyncSystem, DamageEvent>(
		[this](const DamageEvent& event) {
			if (event.source.hasComponent<TagSync>() && event.target.hasComponent<TagSync>())
			{
				auto data = std::make_shared<DamageEventData>();
				data->sourceId = event.source.getComponent<TagSync>().syncId;
				data->targetId = event.target.getComponent<TagSync>().syncId;
				data->damage = event.damage;

				SyncEvent syncEvent(data->getType(), data, Tool::GetTimestampMilliseconds(), m_world ? m_world->getFrameCount() : 0);
				BroadCaseEvent(syncEvent);
			}
		});

	world.Subscribe<ServerSyncSystem, PropPickupEvent>(
		[this](const PropPickupEvent& event) {
			if (event.prop.hasComponent<TagSync>() && event.picker.hasComponent<TagSync>())
			{
				auto data = std::make_shared<PropPickupEventData>();
				data->propId = event.prop.getComponent<TagSync>().syncId;
				data->pickerId = event.picker.getComponent<TagSync>().syncId;
				data->propType = event.propType;

				SyncEvent syncEvent(data->getType(), data, Tool::GetTimestampMilliseconds(), m_world ? m_world->getFrameCount() : 0);
				BroadCaseEvent(syncEvent);
			}
		});

	world.Subscribe<ServerSyncSystem, PickUpHealEvent>(
		[this](const PickUpHealEvent& event) {
			if (event.picker.hasComponent<TagSync>())
			{
				auto data = std::make_shared<PickUpHealEventData>();
				data->pickerId = event.picker.getComponent<TagSync>().syncId;

				SyncEvent syncEvent(data->getType(), data, Tool::GetTimestampMilliseconds(), m_world ? m_world->getFrameCount() : 0);
				BroadCaseEvent(syncEvent);
			}
		});

	world.Subscribe<ServerSyncSystem, WeaponShootEvent>(
		[&world, this](const WeaponShootEvent& event) {
			if (event.source.hasComponent<TagSync>())
			{
				auto data = std::make_shared<WeaponShootEventData>();
				data->sourceId = event.source.getComponent<TagSync>().syncId;

				SyncEvent syncEvent(data->getType(), data, Tool::GetTimestampMilliseconds(), m_world ? m_world->getFrameCount() : 0);
				BroadCaseEvent(syncEvent);
			}
		});
}

void ServerSyncSystem::postUpdate(float dt)
{
	_deltatime_Accumulator += dt;
	if (_deltatime_Accumulator < _update_deltatime_ms)
		return;

	_deltatime_Accumulator -= _update_deltatime_ms;

	BroadCaseGameState();
}

GameState& ServerSyncSystem::GetGameState()
{
	return _gamestate_tripbuffer->acquireReadBuffer();
}

void ServerSyncSystem::SetNetworkMessageSender(std::shared_ptr<NetworkMessageSender>& sender)
{
	_sender = sender;
}

void ServerSyncSystem::SetSyncFps(float fps)
{
	_update_fps = fps;
	_update_deltatime_ms = 1000.f / fps;
	_deltatime_Accumulator = 0.f;
}

float ServerSyncSystem::GetSyncFps()
{
	return _update_fps;
}

void ServerSyncSystem::BroadCaseGameState()
{
	auto& world = getWorld();

	GameState allstate;
	allstate.updateTime = Tool::GetTimestampMilliseconds();
	allstate.framecount = world.getFrameCount();

	std::vector<Entity> entities = world.getEntitiesWith<TagSync>();
	for (auto& entity : entities)
	{
		auto& sync = world.getComponent<TagSync>(entity);

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

	auto& write = _gamestate_tripbuffer->acquireWriteBuffer();
	write = allstate;
	_gamestate_tripbuffer->submitWriteBuffer();

	auto& sender = _sender;
	if (!sender)
		return;

	auto& gamestate = GetGameState();
	if (gamestate.hasConsume)
		return;

	gamestate.hasConsume = true;

	json js;
	js["command"] = GameServiceCommand::GameService_BroadCastGameState;
	js["gamestate"] = gamestate.toJson();

	sender->Broadcast(js);
}

void ServerSyncSystem::BroadCaseEvent(const SyncEvent& event)
{
	auto& sender = _sender;
	if (!sender)
		return;

	json js;
	js["command"] = GameServiceCommand::GameService_SyncGameEvent;
	js["event"] = event.toJson();

	sender->Broadcast(js);
}

void ServerSyncSystem::handleSyncTank(GameState& allstate, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& tankProperty = world.getComponent<TankProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);
	auto& movement = world.getComponent<Movement>(entity);
	auto& health = world.getComponent<Health>(entity);

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

void ServerSyncSystem::handleSyncBullet(GameState& allstate, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& bulletProperty = world.getComponent<BulletProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);
	auto& movement = world.getComponent<Movement>(entity);

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

void ServerSyncSystem::handleSyncProp(GameState& allstate, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& propProperty = world.getComponent<PropProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);

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

void ServerSyncSystem::handleSyncWall(GameState& allstate, Entity entity)
{
	auto& world = getWorld();

	auto& sync = world.getComponent<TagSync>(entity);
	auto& wallproperty = world.getComponent<WallProperty>(entity);
	auto& trans = world.getComponent<Transform>(entity);
	auto& health = world.getComponent<Health>(entity);

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
