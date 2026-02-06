#include "ECS/Factory/PropFactory.h"
#include "ECS/Components/AllComponent.h"

Entity PropFactory::CreateProp(World& world, PropProperty::PropType type, float duration, float x, float y, int width, int height)
{
	auto entity = world.createEntityWithTag<TagProp>();

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;

	auto& prop = entity.addComponent<PropProperty>(type, width, height, duration);

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Static;
	physics.position = { x,y };
	physics.size = { float(width) ,float(height) };
	physics.isSensor = true;
	physics.groupIndex = 0;

	ID2D1Bitmap* bitmap = nullptr;
	if (type == PropProperty::PropType::HEALTH_PACK)
		bitmap = ResFactory->GetBitMapRes(ResName::aidKit);

	entity.addComponent<Sprite>(width, height, bitmap);

	return entity;
}

Entity PropFactory::CreateClientProp(World& world,
	SyncID syncid, PropProperty::PropType type, float duration,
	float x, float y, float rotation,
	int width, int height,
	float lifetime)
{
	auto entity = world.createEntityWithTag<TagProp>();

	auto& sync = entity.addComponent<TagSync>();
	sync.syncId = syncid;
	sync.syncEntityType = TagSync::SyncEntityType::PROP;

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto& prop = entity.addComponent<PropProperty>(type, width, height, duration);

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Static;
	physics.position = { x,y };
	physics.size = { float(width) ,float(height) };
	physics.isSensor = true;
	physics.groupIndex = 0;

	ID2D1Bitmap* bitmap = nullptr;
	if (type == PropProperty::PropType::HEALTH_PACK)
		bitmap = ResFactory->GetBitMapRes(ResName::aidKit);

	entity.addComponent<Sprite>(width, height, bitmap);

	return entity;
}