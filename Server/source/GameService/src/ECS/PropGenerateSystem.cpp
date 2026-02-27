#include "ECS/Systems/PropGenerateSystem.h"
#include "Coroutine.h"

#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"

#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Factory/PropFactory.h"
#include "ECS/Systems/PhysicsSystem.h"

#include <random>
#include "Manager/MapManager.h"

class DelayTask
{
public:
	DelayTask(std::chrono::milliseconds timeout);
	~DelayTask();

	void Wake();
	bool IsDone();

private:
	std::unique_ptr<Task<void>> _task;
	std::shared_ptr<CoTimer> _timer;
};

DelayTask::DelayTask(std::chrono::milliseconds timeout)
{
	_timer = std::make_shared<CoTimer>(timeout);

	auto task = CoroTask::Run([timer = _timer]()->Task<void> {
		if (timer)
			co_await *timer;
		});
	_task = std::make_unique<Task<void>>(std::move(task));
}

DelayTask::~DelayTask()
{
	Wake();
}

void DelayTask::Wake()
{
	if (_timer)
		_timer->wake();

	if (_task)
		_task->sync_wait();
}

bool DelayTask::IsDone()
{
	return _task->is_done();
}

void PropGenerateSystem::onDetach()
{
}

void PropGenerateSystem::preUpdate(float deltaTime)
{
	if (_prepareGenerateTimer)
	{
		if (_prepareGenerateTimer->IsDone())
		{
			doGenerateProp();
			_prepareGenerateTimer.reset();
		}
	}
	else
	{
		auto& world = getWorld();
		std::vector<Entity> entities = world.getEntitiesWith<TagProp, PropProperty>();
		if (entities.size() < _config.maxProps)
			prepareGenerateProp();
	}
}

void PropGenerateSystem::prepareGenerateProp()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(_config.minDelayMs, _config.maxDelayMs);
	int randomTimeMS = dist(gen);

	_prepareGenerateTimer = std::make_shared<DelayTask>(std::chrono::milliseconds(randomTimeMS));
}

void PropGenerateSystem::doGenerateProp()
{
	static auto avaliablePropType =
		std::vector<PropProperty::PropType>{ PropProperty::PropType::HEALTH_PACK };

	if (!m_world || avaliablePropType.empty())
		return;

	PropProperty::PropType selectPropType;

	if (avaliablePropType.size() > 1)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, avaliablePropType.size() - 1);
		int randomIndex = dist(gen);
		selectPropType = avaliablePropType[randomIndex];
	}
	else
		selectPropType = avaliablePropType[0];

	float x, y;
	float rotation = 0.f;
	float duration = 5.f;
	int width = _config.propWidth, height = _config.propHeight;
	float lifetime = _config.propLifetime;

	if (GetAvaliablePropPosition(x, y, width, height))
		PropFactory::CreateServerProp(*m_world, selectPropType, duration, x, y, rotation, width, height, lifetime);
}

bool PropGenerateSystem::GetAvaliablePropPosition(float& x, float& y, int width, int height)
{
	if (!m_world)
		return false;

	PhysicsSystem* physicsSystem = m_world->getSystem<PhysicsSystem>();
	if (!physicsSystem)
		return false;

	auto isWithinMapBounds = [](float x, float y)->bool {
		return
			x > MapBoundary::left &&
			x < MapBoundary::right &&
			y > MapBoundary::top &&
			y < MapBoundary::bottom;
		};


	auto generateRandomPosition = [](float& x, float& y)-> void {
		std::random_device rd;
		std::mt19937 gen(rd());

		{
			std::uniform_real_distribution<float> dist(MapBoundary::left, MapBoundary::right);
			x = dist(gen);
		}

		{
			std::uniform_real_distribution<float> dist(MapBoundary::top, MapBoundary::bottom);
			y = dist(gen);
		}
		};

	int trycount = 0;
	while (trycount < 5)
	{
		float tempX, tempY;
		generateRandomPosition(tempX, tempY);

		auto results = physicsSystem->QueryAABB(Pos2(tempX - width / 2, tempY - height / 2), Pos2(tempX + width / 2, tempY + height / 2));
		if (!results.empty() || !isWithinMapBounds(tempX, tempY))
		{
			trycount++;
			continue;
		}

		x = tempX;
		y = tempY;
		return true;
	}

	return false;
}