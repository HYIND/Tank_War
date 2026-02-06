#include "ECS/Factory/TankFactory.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Event/ECSEventDef.h"
#include "stdafx.h"

Entity TankFactory::CreateServerTank(World &world,
									 TankProperty::TankOwner owner, PlayerID playerid,
									 float x, float y,
									 int width, int height)
{
	Entity tank = world.createEntityWithTag<TagTank>();

	auto &sync = tank.addComponent<TagSync>();
	sync.syncId = Tool::GenerateSimpleUuid();
	sync.syncEntityType = TagSync::SyncEntityType::TANK;

	auto &trans = tank.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;

	tank.addComponent<PlayerInput>();
	tank.addComponent<Controller>();
	auto &move = tank.addComponent<Movement>();
	move.maxMoveSpeed = 300.f;
	move.maxRotationSpeed = 180.f;

	tank.addComponent<TankProperty>(owner, playerid, width, height);
	tank.addComponent<Health>();

	auto &weapon = tank.addComponent<Weapon>();
	weapon.bulletSpeed = 500.f;

	auto &tankvisual = tank.addComponent<TankVisual>(TankVisual::VisualState::BASIC, width, height);
	tankvisual.layer = (int)RenderLayer::TankLayer;

	auto &physics = tank.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = {x, y};
	physics.size = {width, height};
	physics.isSensor = false;
	physics.isBullet = true;
	physics.groupIndex = getPhysicsGroupIndex();

	return tank;
}

int16 TankFactory::getPhysicsGroupIndex()
{
	static int16 index = -1;
	return index--;
}
