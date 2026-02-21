#include "ECS/Factory/TankFactory.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Event/ECSEventDef.h"

Entity TankFactory::CreateLocalGameTank(World& world,
	TankProperty::TankOwner owner, float x, float y,
	int width, int height
)
{
	Entity tank = world.createEntityWithTag<TagTank>();

	auto& trans = tank.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;

	tank.addComponent<PlayerInput>();
	tank.addComponent<Controller>();
	auto& move = tank.addComponent<Movement>();
	move.maxMoveSpeed = 300.f;
	move.maxRotationSpeed = 180.f;

	tank.addComponent<TankProperty>(owner, width, height);
	tank.addComponent<Health>();

	auto& weapon = tank.addComponent<Weapon>();
	weapon.bulletSpeed = 500.f;

	auto& tankvisual = tank.addComponent<TankVisual>(TankVisual::VisualState::BASIC, width, height);
	tankvisual.layer = (int)RenderLayer::TankLayer;
	auto& healthshow = tank.addComponent<HealthShow>(std::max(10, width - 12), 3);
	healthshow.layer = (int)RenderLayer::HealthShow;

	if (owner == TankProperty::TankOwner::AI)
	{
		auto& aicontrol = tank.addComponent<AIControl>();
		aicontrol.decisionIntervalms = 125.f;
	}


	auto& physics = tank.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = { x,y };
	physics.size = { (float)width ,(float)height };
	physics.isSensor = false;
	physics.isBullet = true;
	physics.groupIndex = getPhysicsGroupIndex();

	return tank;
}

Entity TankFactory::CreateClientTank(World& world,
	TankProperty::TankOwner owner, SyncID syncid, PlayerID playerid,
	float x, float y, int width, int height,
	float rotation
)
{
	Entity tank = world.createEntityWithTag<TagTank>();

	auto& sync = tank.addComponent<TagSync>();
	sync.syncId = syncid;
	sync.syncEntityType = TagSync::SyncEntityType::TANK;

	auto& trans = tank.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	tank.addComponent<PlayerInput>();
	tank.addComponent<Controller>();
	auto& move = tank.addComponent<Movement>();
	move.maxMoveSpeed = 300.f;
	move.maxRotationSpeed = 180.f;

	tank.addComponent<TankProperty>(owner, playerid, width, height);
	tank.addComponent<Health>();

	auto& weapon = tank.addComponent<Weapon>();
	weapon.bulletSpeed = 500.f;

	auto& tankvisual = tank.addComponent<TankVisual>(TankVisual::VisualState::BASIC, width, height);
	tankvisual.layer = (int)RenderLayer::TankLayer;
	auto& healthshow = tank.addComponent<HealthShow>(std::max(10, width - 12), 3);
	healthshow.layer = (int)RenderLayer::HealthShow;

	auto& physics = tank.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = { x,y };
	physics.size = { (float)width ,(float)height };
	physics.isSensor = false;
	physics.isBullet = true;
	physics.groupIndex = getPhysicsGroupIndex();

	auto& interpolation = tank.addComponent<Interpolation>();

	return tank;
}

int16 TankFactory::getPhysicsGroupIndex()
{
	static int16 index = -1;
	return index--;
}
