#include "ECS/Systems/WeaponSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Event/ECSEventDef.h"
#include "ECS/Factory/BulletFactory.h"
#include <algorithm>

void WeaponSystem::onAttach(World& world)
{
	m_world->Subscribe<WeaponSystem, BulletDestroyedEvent>(
		[&](const BulletDestroyedEvent& event)->void
		{
			Entity entity = event.shooter;
			if (auto* weapon = entity.tryGetComponent<Weapon>())
				weapon->currentBullets = std::max(0, weapon->currentBullets - 1);
		}
	);
}

void WeaponSystem::update(float deltaTime)
{
	auto entities = m_world->getEntitiesWith<Controller, Weapon, Transform>();

	for (auto& entity : entities)
	{
		auto& weapon = entity.getComponent<Weapon>();
		if (weapon.cooldown > 0)
			weapon.cooldown = std::max(0.f, float(weapon.cooldown) - deltaTime);

		auto& controller = entity.getComponent<Controller>();
		if (controller.wantToFire && weapon.cooldown <= 0 && weapon.currentBullets < weapon.maxBullets)
		{
			auto& trans = entity.getComponent<Transform>();
			BulletFactory::CreateLocalBullet(*m_world, entity, weapon.bulletDamage, weapon.bulletSpeed, trans.position.x, trans.position.y, 20, trans.rotation);
			weapon.cooldown = weapon.fireRate;
			weapon.currentBullets++;
		}
	}
}
