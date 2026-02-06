#include "ECS/Factory/WallFactory.h"
#include "ECS/Components/AllComponent.h"
#include "tools.h"

Entity WallFactory::CreateServerWall(World &world,
									 WallProperty::WallType type,
									 float x, float y, float rotation,
									 int width, int height,
									 int maxhealth)
{
	auto entity = world.createEntityWithTag<TagWall>();

	auto &sync = entity.addComponent<TagSync>();
	sync.syncId = Tool::GenerateSimpleUuid();
	sync.syncEntityType = TagSync::SyncEntityType::WALL;

	auto &trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	entity.addComponent<WallProperty>(type, width, height);
	
	auto &health = entity.addComponent<Health>(maxhealth);
	health.isInvulnerable = type == WallProperty::WallType::IRON;

	auto &physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Static;
	physics.position = {x, y};
	physics.size = {float(width), float(height)};
	physics.isSensor = false;
	physics.groupIndex = 0;

	return entity;
}
