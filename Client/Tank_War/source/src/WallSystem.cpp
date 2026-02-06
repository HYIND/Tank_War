#include "ECS/Systems/WallSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"
#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

void WallSystem::onAttach(World& world)
{
	m_world->Subscribe<WallSystem, EntityDeathEvent>(
		[&](const EntityDeathEvent& event)->void
		{
			auto entity = event.entity;
			if (entity.hasComponent<TagWall>() && entity.hasComponent<WallProperty>())
			{
				WallDestroyedEvent wallevent;
				wallevent.wall = entity;
				wallevent.killer = event.killer;
				wallevent.position = event.deathPosition;
				m_world->destroyEntityLaterWithEvent<WallDestroyedEvent>(entity, wallevent);
			}
		}
	);
}
