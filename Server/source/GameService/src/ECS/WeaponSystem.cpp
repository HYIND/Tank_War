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
			if (auto* bulletcore = event.bullet.tryGetComponent<BulletCore>())
			{
				if (auto* weaponcontainer = event.shooter.tryGetComponent<WeaponContainer>())
				{
					if (auto* weaponconfig = weaponcontainer->tryGetWeapon(bulletcore->type))
					{
						if (weaponconfig->durationType != WeaponDurationType::CountTemporary)
							weaponconfig->currentBullets = std::max(0, weaponconfig->currentBullets - 1);
					}
				}
			}
		}
	);
}

void WeaponSystem::update(float deltaTime)
{
	auto entities = m_world->getEntitiesWith<Controller, WeaponContainer, Transform>();

	for (auto& entity : entities)
	{
		auto& weaponcontainer = entity.getComponent<WeaponContainer>();

		weaponcontainer.changeWeapon_cooldown = std::max(0.f, float(weaponcontainer.changeWeapon_cooldown) - deltaTime);

		if (weaponcontainer.weapons.empty())
			continue;

		for (auto it : weaponcontainer.weapons)
		{
			if (auto weaponconfig = it.second)
				weaponconfig->cooldown = std::max(0.f, float(weaponconfig->cooldown) - deltaTime);
		}

		if (weaponcontainer.changeWeapon_cooldown > 0.f)
			continue;

		static std::vector<WeaponType> checkTypeOrders = { WeaponType::EnergyWave ,WeaponType::Default };
		for (auto weapontype : checkTypeOrders)
		{
			if (auto* weaponconfig = weaponcontainer.tryGetWeapon(weapontype))
			{
				auto& controller = entity.getComponent<Controller>();
				if (controller.wantToFire && weaponconfig->cooldown <= 0 && weaponconfig->currentBullets < weaponconfig->maxBullets)
				{
					PlayerID playerid;
					if (auto* tankproperty = entity.tryGetComponent<TankProperty>())
						playerid = tankproperty->playerId;

					auto& trans = entity.getComponent<Transform>();
					if (weaponconfig->type == WeaponType::Default)
						BulletFactory::CreateServerDefaultBullet(*m_world,
							entity, playerid,
							weaponconfig->bulletDamage, weaponconfig->bulletSpeed,
							trans.position.x, trans.position.y, 20, trans.rotation);
					else if (weaponconfig->type == WeaponType::EnergyWave)
						BulletFactory::CreateServerEnergyWaveBullet(*m_world,
							entity, playerid,
							weaponconfig->bulletDamage, weaponconfig->bulletSpeed,
							trans.position.x, trans.position.y, 80, 35, trans.rotation);

					weaponconfig->cooldown = weaponconfig->fireRate;
					weaponconfig->currentBullets++;


					if (weaponconfig->durationType == WeaponDurationType::CountTemporary)
					{
						if (weaponconfig->currentBullets >= weaponconfig->maxBullets)
						{
							weaponcontainer.removeWeapon(weaponconfig->type);
							weaponcontainer.changeWeapon_cooldown = weaponcontainer.changeWeapon_Time;
						}
					}

					m_world->Emit<WeaponShootEvent>(WeaponShootEvent{ .source = entity });

				}

				break;
			}
		}
	}
}
