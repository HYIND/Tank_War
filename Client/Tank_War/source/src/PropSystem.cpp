#include "ECS/Systems/PropSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"

#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

void PropSystem::onAttach(World& world)
{
	m_world->Subscribe<PropSystem, PhysicsCollisionEvent>(
		[&](const PhysicsCollisionEvent& event)-> void
		{
			auto entityA = event.entityA;
			auto entityB = event.entityB;

			bool isA_prop = entityA.hasComponent<TagProp>() && entityA.hasComponent<PropProperty>();
			bool isB_prop = entityB.hasComponent<TagProp>() && entityB.hasComponent<PropProperty>();

			if (isA_prop && !isB_prop)
			{
				processPropCollision(entityA, entityB, event.point);
			}
			else if (isB_prop && !isA_prop)
			{
				processPropCollision(entityB, entityA, event.point);
			}
		}
	);
}

void PropSystem::processPropCollision(Entity prop, Entity picker, Pos2 point)
{
	if (!picker.hasComponent<TagTank>())
		return;

	if (auto* propProperty = prop.tryGetComponent<PropProperty>())
	{
		bool pickup = false;
		if (propProperty->type == PropProperty::PropType::HEALTH_PACK)
			pickup = pickUpHealthPack(prop, picker);
		else if (propProperty->type == PropProperty::PropType::ENERGY_WAVE)
			pickup = pickUpEnergyWave(prop, picker);

		if (pickup)
		{
			m_world->Emit(PropPickupEvent{
				.prop = prop,
				.picker = picker,
				.propType = propProperty->type
				});
			m_world->destroyEntityLater(prop);
		}
	}
};

bool PropSystem::pickUpHealthPack(Entity prop, Entity picker)
{
	auto* health = picker.tryGetComponent<Health>();
	if (!health)
		return false;

	auto& propProperty = prop.getComponent<PropProperty>();

	if (health->currentHealth < health->maxHealth)
	{
		health->heal(float(health->maxHealth) / 2.f);

		m_world->Emit(PickUpHealEvent{
			.picker = picker,
			});

		return true;
	}
	else
	{
		return false;
	}
}

bool PropSystem::pickUpEnergyWave(Entity prop, Entity picker)
{
	auto* weaponcontainer = picker.tryGetComponent<WeaponContainer>();
	if (!weaponcontainer)
		return false;

	if (auto w = weaponcontainer->tryGetWeapon(WeaponType::EnergyWave))
		return false;

	auto weaponconfig = new WeaponConfig(WeaponType::EnergyWave);
	weaponconfig->durationType = WeaponDurationType::CountTemporary;
	weaponconfig->bulletSpeed = 400.f;
	weaponconfig->maxBullets = 1;
	weaponcontainer->addWeapon(weaponconfig);

	return true;
}

