#include "ECS/Factory/BulletFactory.h"
#include "ECS/Components/AllComponent.h"

Entity BulletFactory::CreateLocalDefaultBullet(World& world, Entity owner, int bulletDamage, float bulletSpeed,
	float x, float y, int radius,
	float rotation
)
{
	if (!owner)
		return Entity();

	auto entity = world.createEntityWithTag<TagBullet>();

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto& move = entity.addComponent<Movement>(bulletSpeed, 0);
	move.setCurrentMoveSpeed(move.maxMoveSpeed);

	entity.addComponent<BulletCore>(WeaponType::Default, owner, bulletDamage);
	entity.addComponent<CircleShape>(radius);
	entity.addComponent<BounceAbility>();

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Round;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = { x,y };
	physics.size = { float(radius) ,float(radius) };
	physics.isSensor = false;
	physics.isBullet = true;
	physics.density = 0.01f;
	physics.friction = 0.0f;
	physics.restitution = 1.0f;

	if (owner.hasComponent<Physics>())
		physics.groupIndex = owner.getComponent<Physics>().groupIndex;
	physics.categoryBits = CATEGORY_BULLET;
	physics.maskBits = ~CATEGORY_BULLET;

	entity.addComponent<Sprite>(radius, radius, ResFactory->GetBitMapRes(ResName::defBullet));

	auto& lifetime = entity.addComponent<LifeTime>(5);

	return entity;
}


Entity BulletFactory::CreateClientDefaultBullet(World& world,
	SyncID syncid, Entity owner, PlayerID playerid,
	int bulletDamage, float bulletSpeed,
	float x, float y, int radius,
	float rotation
)
{

	auto entity = world.createEntityWithTag<TagBullet>();

	auto& sync = entity.addComponent<TagSync>();
	sync.syncId = syncid;
	sync.syncEntityType = TagSync::SyncEntityType::BULLET;

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto& move = entity.addComponent<Movement>(bulletSpeed, 0);
	move.setCurrentMoveSpeed(move.maxMoveSpeed);

	entity.addComponent<BulletCore>(WeaponType::Default, playerid, bulletDamage);
	entity.addComponent<CircleShape>(radius);
	entity.addComponent<BounceAbility>();

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Round;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = { x,y };
	physics.size = { float(radius) ,float(radius) };
	physics.isSensor = false;
	physics.isBullet = true;
	physics.density = 0.01f;
	physics.friction = 0.0f;
	physics.restitution = 1.0f;

	if (world.IsValidWorldEntity(owner) && owner.hasComponent<Physics>())
		physics.groupIndex = owner.getComponent<Physics>().groupIndex;

	auto& interpolation = entity.addComponent<Interpolation>();

	entity.addComponent<Sprite>(radius, radius, ResFactory->GetBitMapRes(ResName::defBullet));

	return entity;
}

Entity BulletFactory::CreateLocalEnergyWaveBullet(World& world, Entity owner, int bulletDamage, float bulletSpeed,
	float x, float y, float width, float height,
	float rotation
)
{
	if (!owner)
		return Entity();

	auto entity = world.createEntityWithTag<TagBullet>();

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto& move = entity.addComponent<Movement>(bulletSpeed, 0);
	move.setCurrentMoveSpeed(move.maxMoveSpeed);

	entity.addComponent<BulletCore>(WeaponType::EnergyWave, owner, bulletDamage);
	entity.addComponent<RectShape>(width, height);

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Dynamic;
	physics.position = { x,y };
	physics.size = { float(width) ,float(height) };
	physics.isSensor = true;
	physics.isBullet = true;

	if (owner.hasComponent<Physics>())
		physics.groupIndex = owner.getComponent<Physics>().groupIndex;
	physics.categoryBits = CATEGORY_BULLET;
	physics.maskBits = ~CATEGORY_BULLET;

	auto& gif = entity.addComponent<GIFAnimator>(width, height, ResFactory->GetGIFRes(ResName::energywaveGIF), 0);

	auto& lifetime = entity.addComponent<LifeTime>(5);

	return entity;
}


Entity BulletFactory::CreateClientEnergyWaveBullet(World& world,
	SyncID syncid, Entity owner, PlayerID playerid,
	int bulletDamage, float bulletSpeed,
	float x, float y, float width, float height,
	float rotation
)
{

	auto entity = world.createEntityWithTag<TagBullet>();

	auto& sync = entity.addComponent<TagSync>();
	sync.syncId = syncid;
	sync.syncEntityType = TagSync::SyncEntityType::BULLET;

	auto& trans = entity.addComponent<Transform>();
	trans.position.x = x;
	trans.position.y = y;
	trans.rotation = rotation;

	auto& move = entity.addComponent<Movement>(bulletSpeed, 0);
	move.setCurrentMoveSpeed(move.maxMoveSpeed);

	entity.addComponent<BulletCore>(WeaponType::EnergyWave, owner, bulletDamage);
	entity.addComponent<RectShape>(width, height);

	auto& physics = entity.addComponent<Physics>();
	physics.shape = Physics::Shape::Rect;
	physics.bodyType = Physics::BodyType::Kinematic;
	physics.position = { x,y };
	physics.size = { float(width) ,float(height) };
	physics.isSensor = true;
	physics.isBullet = true;

	if (world.IsValidWorldEntity(owner) && owner.hasComponent<Physics>())
		physics.groupIndex = owner.getComponent<Physics>().groupIndex;

	auto& interpolation = entity.addComponent<Interpolation>();

	auto& gif = entity.addComponent<GIFAnimator>(width, height, ResFactory->GetGIFRes(ResName::energywaveGIF), 0);

	return entity;
}