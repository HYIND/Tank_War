#include "ECS/Systems/EffectSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"

#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

void EffectSystem::onAttach(World& world)
{
	m_world->Subscribe<EffectSystem, TankDestroyedEvent>(
		[&](const TankDestroyedEvent& event)->void
		{
			auto tank = event.tank;
			auto killer = event.killer;
			if (tank.hasComponent<Transform>() && tank.hasComponent<TankVisual>() && tank.hasComponent<TagTank>())
			{
				auto& trans = tank.getComponent<Transform>();
				auto& visual = tank.getComponent<TankVisual>();
				spawnExplosion(trans.position, visual.width, visual.height);
			}
		}
	);
}

void EffectSystem::spawnExplosion(Pos2 position, int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	Entity explosion = m_world->createEntity();

	explosion.addComponent<Transform>(position);
	explosion.addComponent<TagEffect>();  // 标记是爆炸
	auto& gif = explosion.addComponent<GIFAnimator>(width * 1.5, height * 1.5, ResFactory->GetGIFRes(ResName::explosionGIF));
	gif.internalZOrder = 100;
	explosion.addComponent<LifeTime>(ResFactory->GetGIFRes(ResName::explosionGIF)->getDefaultMsTime() / 1000.f);
}
