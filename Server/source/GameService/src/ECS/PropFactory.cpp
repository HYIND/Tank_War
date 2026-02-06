#include "ECS/Factory/PropFactory.h"
#include "ECS/Components/AllComponent.h"
#include "tools.h"

Entity PropFactory::CreateServerProp(World &world,
									 PropProperty::PropType type, float duration,
									 float x, float y, float rotation,
									 int width, int height,
									 float lifetime)
{
	auto entity = world.createEntityWithTag<TagProp>();

	auto &sync = entity.addComponent<TagSync>();
	sync.syncId = Tool::GenerateSimpleUuid();
	sync.syncEntityType = TagSync::SyncEntityType::PROP;

	auto &trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto &prop = entity.addComponent<PropProperty>(type, width, height, duration);

	auto &physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Static;
	physics.position = {x, y};
	physics.size = {float(width), float(height)};
	physics.isSensor = true;
	physics.groupIndex = 0;

	if (lifetime > 0.f)
		entity.addComponent<LifeTime>(lifetime);

	return entity;
}
