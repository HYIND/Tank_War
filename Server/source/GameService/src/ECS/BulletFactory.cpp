#include "ECS/Factory/BulletFactory.h"
#include "ECS/Components/AllComponent.h"
#include "stdafx.h"

Entity BulletFactory::CreateServerBullet(World &world, Entity owner, PlayerID playerid, int bulletDamage, float bulletSpeed,
										 float x, float y, int radius,
										 float rotation)
{
	if (!owner)
		return Entity();

	auto entity = world.createEntityWithTag<TagBullet>();

	auto &sync = entity.addComponent<TagSync>();
	sync.syncId = Tool::GenerateSimpleUuid();
	sync.syncEntityType = TagSync::SyncEntityType::BULLET;

	auto &trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto &move = entity.addComponent<Movement>(bulletSpeed, 0);
	move.setCurrentMoveSpeed(move.maxMoveSpeed);

	entity.addComponent<BulletProperty>(owner, playerid, radius, bulletDamage);

	auto &physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Round;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = {x, y};
	physics.size = {float(radius), float(radius)};
	physics.isSensor = false;
	physics.isBullet = true;
	physics.density = 0.01f;
	physics.friction = 0.0f;
	physics.restitution = 1.0f;

	if (owner.hasComponent<Physics>())
		physics.groupIndex = owner.getComponent<Physics>().groupIndex;

	auto &lifetime = entity.addComponent<LifeTime>(10);

	return entity;
}
