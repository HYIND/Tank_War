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
			if (entity.hasComponent<TagBullet>() && entity.hasComponent<BulletCore>())
			{
				auto& bulletCore = entity.getComponent<BulletCore>();
				BulletDestroyedEvent bulletDestroyedEvent{
					.bullet = entity,
					.shooter = bulletCore.owner
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

			bool isA_bullet = entityA.hasComponent<TagBullet>() && entityA.hasComponent<BulletCore>();
			bool isB_bullet = entityB.hasComponent<TagBullet>() && entityB.hasComponent<BulletCore>();

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
	if ((!health || health->isInvulnerable) && bullet.hasComponent<BounceAbility>())
	{
		processBulletBounce(bullet);
		return;
	}

	auto& bulletCore = bullet.getComponent<BulletCore>();
	m_world->Emit<DamageEvent>(DamageEvent{
		.target = target,
		.source = bullet,
		.damage = bulletCore.damage
		});

	if (bulletCore.type != WeaponType::EnergyWave)
		m_world->destroyEntityLater(bullet);
};

void BulletSystem::processBulletBounce(Entity bullet)
{
	auto& bulletBounceAbility = bullet.getComponent<BounceAbility>();
	bulletBounceAbility.bounceCount++;
	if (bulletBounceAbility.bounceCount > bulletBounceAbility.maxBounces)
		m_world->destroyEntityLater(bullet);
}
