#include "ECS/Factory/WallFactory.h"
#include "ECS/Components/AllComponent.h"

Entity WallFactory::CreateWall(World& world, WallProperty::WallType type, float x, float y, int width, int height)
{
	auto entity = world.createEntityWithTag<TagWall>();

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;

	entity.addComponent<WallProperty>(type, width, height);
	auto& health = entity.addComponent<Health>(80);
	health.isInvulnerable = type == WallProperty::WallType::IRON;

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Static;
	physics.position = { x,y };
	physics.size = { float(width) ,float(height) };
	physics.isSensor = false;
	physics.groupIndex = 0;

	ID2D1Bitmap* bitmap = nullptr;
	if (type == WallProperty::WallType::BRICK)
		bitmap = ResFactory->GetBitMapRes(ResName::brickWall);
	else if (type == WallProperty::WallType::IRON)
		bitmap = ResFactory->GetBitMapRes(ResName::ironWall);

	entity.addComponent<Sprite>(width, height, bitmap);

	return entity;
}

Entity WallFactory::CreateClientWall(World& world,
	SyncID syncid, WallProperty::WallType type,
	float x, float y, float rotation,
	int width, int height,
	int currenthealth, int maxhealth)
{
	auto entity = world.createEntityWithTag<TagWall>();

	auto& sync = entity.addComponent<TagSync>();
	sync.syncId = syncid;
	sync.syncEntityType = TagSync::SyncEntityType::WALL;

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	entity.addComponent<WallProperty>(type, width, height);
	auto& health = entity.addComponent<Health>(maxhealth);
	health.currentHealth = currenthealth;
	health.isInvulnerable = type == WallProperty::WallType::IRON;

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Static;
	physics.position = { x,y };
	physics.size = { float(width) ,float(height) };
	physics.isSensor = false;
	physics.groupIndex = 0;

	ID2D1Bitmap* bitmap = nullptr;
	if (type == WallProperty::WallType::BRICK)
		bitmap = ResFactory->GetBitMapRes(ResName::brickWall);
	else if (type == WallProperty::WallType::IRON)
		bitmap = ResFactory->GetBitMapRes(ResName::ironWall);

	entity.addComponent<Sprite>(width, height, bitmap);

	return entity;
}
