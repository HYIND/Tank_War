#include "ECS/Systems/TankSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"
#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

void TankSystem::onAttach(World& world)
{
	m_world->Subscribe<TankSystem, EntityDeathEvent>(
		[&](const EntityDeathEvent& event)->void
		{
			auto entity = event.entity;
			if (entity.hasComponent<TagTank>() && entity.hasComponent<Transform>())
			{
				TankDestroyedEvent tankevent;
				tankevent.tank = entity;
				tankevent.killer = event.killer;
				tankevent.position = event.deathPosition;
				m_world->destroyEntityLaterWithEvent<TankDestroyedEvent>(entity, tankevent);
			}
		}
	);
}
