#include "ECS/Systems/BulletSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"

#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

void BulletSystem::onAttach(World& world)
{
	m_world->Subscribe<BulletSystem, EntityDestroyedEvent>(
		[&](const EntityDestroyedEvent& event)->void
		{
			auto entity = event.entity;
			if (entity.hasComponent<TagBullet>() && entity.hasComponent<BulletProperty>())
			{
				auto& bulletProperty = entity.getComponent<BulletProperty>();
				BulletDestroyedEvent bulletDestroyedEvent{
					.bullet = entity,
					.shooter = bulletProperty.owner
				};
				if (auto* trans = entity.tryGetComponent<Transform>())
					bulletDestroyedEvent.position = trans->position;
				if (entity.hasComponent<TagLifeTimeOut>())
					bulletDestroyedEvent.reason = BulletDestroyedEvent::DestroyReason::Timeout;
				m_world->Emit<BulletDestroyedEvent>(bulletDestroyedEvent);
			}
		}
	);

	m_world->Subscribe<BulletSystem, PhysicsCollisionEvent>(
		[&](const PhysicsCollisionEvent& event)-> void
		{
			auto entityA = event.entityA;
			auto entityB = event.entityB;

			bool isA_bullet = entityA.hasComponent<TagBullet>() && entityA.hasComponent<BulletProperty>();
			bool isB_bullet = entityB.hasComponent<TagBullet>() && entityB.hasComponent<BulletProperty>();

			if (isA_bullet && !isB_bullet)
			{
				processBulletCollision(entityA, entityB, event.point);
			}
			else if (isB_bullet && !isA_bullet)
			{
				processBulletCollision(entityB, entityA, event.point);
			}
		}
	);
}

void BulletSystem::processBulletCollision(Entity bullet, Entity target, Pos2 point)
{
	if (target.hasComponent<TagProp>())
		return;

	auto* health = target.tryGetComponent<Health>();
	if (!health || health->isInvulnerable)
	{
		processBulletBounce(bullet);
		return;
	}

	auto& bulletproperty = bullet.getComponent<BulletProperty>();
	m_world->Emit<DamageEvent>(DamageEvent{
		.target = target,
		.source = bullet,
		.damage = bulletproperty.bulletDamage
		});
	m_world->destroyEntityLater(bullet);
};

void BulletSystem::processBulletBounce(Entity bullet)
{
	auto& bulletproperty = bullet.getComponent<BulletProperty>();
	bulletproperty.bounceCount++;
	if (bulletproperty.bounceCount > bulletproperty.maxBounces)
		m_world->destroyEntityLater(bullet);
}
