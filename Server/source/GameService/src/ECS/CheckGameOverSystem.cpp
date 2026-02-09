#include "ECS/Systems/CheckGameOverSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"
#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

CheckGameOverSystem::CheckGameOverSystem()
{
	SetGameTimeLimit(300.f);
}

CheckGameOverSystem::~CheckGameOverSystem()
{
}

void CheckGameOverSystem::onAttach(World &world)
{
	m_world->Subscribe<CheckGameOverSystem, TankDestroyedEvent>(
		[&](const TankDestroyedEvent &event) -> void
		{
			handleProcessKillEvent(event);
		});
}

void CheckGameOverSystem::preUpdate(float deltaTime)
{
	_gameTimeLimit -= deltaTime;
	if (_gameTimeLimit <= 0)
	{
		if (m_instance)
			m_instance->GameOver();
	}
}

void CheckGameOverSystem::handleProcessKillEvent(const TankDestroyedEvent &event)
{
	if (!m_instance)
		return;

	auto entity = event.tank;
	if (entity.hasComponent<TagTank>() && entity.hasComponent<TankProperty>())
	{
		{
			auto &tankproperty = entity.getComponent<TankProperty>();
			if (!tankproperty.owner != TankProperty::TankOwner::AI && !tankproperty.playerId.empty())
			{
				std::string killerId;
				if (event.killer)
				{
					auto killer = event.killer;
					if (killer->hasComponent<TagBullet>() && killer->hasComponent<BulletProperty>())
					{
						auto &bulletproperty = killer->getComponent<BulletProperty>();
						if (m_world->IsValidWorldEntity(bulletproperty.owner))
						{
							if (bulletproperty.owner.hasComponent<TagTank>() && bulletproperty.owner.hasComponent<TankProperty>())
							{
								auto owner_tankproperty = bulletproperty.owner.getComponent<TankProperty>();
								if (owner_tankproperty.owner == TankProperty::TankOwner::AI)
								{
									killerId = "AI";
								}
								else if (!owner_tankproperty.playerId.empty())
								{
									killerId = owner_tankproperty.playerId;
								}
							}
						}
					}
				}
				m_instance->PlayerEliminated(tankproperty.playerId, killerId);
			}
		}

		auto entities = m_world->getEntitiesWith<TagTank, TankProperty>();

		for (auto it = entities.begin(); it != entities.end(); it++)
		{
			if (*it == event.tank)
			{
				entities.erase(it);
				break;
			}
		}

		{
			if (entities.empty())
			{
				m_instance->GameOver();
				return;
			}
		}

		{
			if (entities.size() == 1)
			{
				auto &winner = entities[0];
				auto &tankproperty = winner.getComponent<TankProperty>();
				if (!tankproperty.owner != TankProperty::TankOwner::AI && !tankproperty.playerId.empty())
				{
					m_instance->GameOverWithWinner(tankproperty.playerId);
				}
				else
				{
					m_instance->GameOver();
				}
			}
		}
	}
}

void CheckGameOverSystem::SetInstance(std::shared_ptr<GameInstance> instance)
{
	m_instance = instance;
}

void CheckGameOverSystem::SetGameTimeLimit(uint64_t gameTimeLimitSecond)
{
	_gameTimeLimit = float(gameTimeLimitSecond) * 1000.f;
}
