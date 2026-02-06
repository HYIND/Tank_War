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
		if (propProperty->type == PropProperty::PropType::HEALTH_PACK)
			processHealthPack(prop, picker);

		m_world->Emit(PropPickupEvent{
			.prop = prop,
			.picker = picker,
			.propType = propProperty->type
			});
		m_world->destroyEntityLater(prop);
	}
};

void PropSystem::processHealthPack(Entity prop, Entity picker)
{
	auto& propProperty = prop.getComponent<PropProperty>();
	if (auto* health = picker.tryGetComponent<Health>())
		health->heal(float(health->maxHealth) / 2.f);
}
