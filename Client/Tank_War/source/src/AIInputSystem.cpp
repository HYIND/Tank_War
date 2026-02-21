#include "ECS/Systems/AIInputSystem.h"
#include "ECS/Systems/AllSystem.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"
#include <ranges>
#include <numeric>
#include <random>

class RandomBool {
public:
	static bool get(double p = 0.5)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::bernoulli_distribution dist(p);
		return dist(gen);
	}
};

static float normalizeDegrees(float angle) {
	angle = std::fmod(angle, 360.0);
	if (angle > 180.0) angle -= 360.0;
	if (angle < -180.0) angle += 360.0;
	return angle;
}

void AIInputSystem::preUpdate(float deltaTime)
{
	auto& world = getWorld();
	std::vector<Entity> entities = world.getEntitiesWith<AIControl, TankProperty, Controller, Transform, Movement>();
	for (auto& entity : entities)
	{
		auto& aicontrol = entity.getComponent<AIControl>();
		aicontrol.timeAccumulator += deltaTime;
		if (aicontrol.timeAccumulator >= aicontrol.decisionIntervalms)
		{
			aicontrol.timeAccumulator = 0.f;
			handleAIInputToTank(entity);
		}
		//drawDebugInfo(entity);
	}
}

void AIInputSystem::handleAIInputToTank(Entity AITank)
{
	float target_factor = 1.f;
	float wall_factor = 1.f;

	auto& world = getWorld();

	auto& aiControl = world.getComponent<AIControl>(AITank);
	auto& tankProperty = world.getComponent<TankProperty>(AITank);
	auto& controller = world.getComponent<Controller>(AITank);
	auto& trans = world.getComponent<Transform>(AITank);

	auto alltargets = world.getEntitiesWith<TagTank, TankProperty, Transform>() |
		std::views::filter([&AITank](Entity entity) {
		auto* tankProperty = entity.tryGetComponent<TankProperty>();
		if (!tankProperty) return false;
		return entity != AITank;
			});

	if (alltargets.empty())
	{
		controller.setWantToFire(false);
		controller.setWantToRotate(Controller::RotationDirection::NONE);
		controller.setWantToMove(Controller::MoveDirection::NONE);
		return;
	}

	auto target = *std::ranges::min_element(alltargets, {},
		[&trans](Entity& e) {
			if (auto* targettrans = e.tryGetComponent<Transform>())
				return PosDistance(trans.position, targettrans->position);
			return FLT_MAX;
		});

	if (!target.hasComponent<Transform>())
	{
		controller.setWantToFire(false);
		controller.setWantToRotate(Controller::RotationDirection::NONE);
		controller.setWantToMove(Controller::MoveDirection::NONE);
		return;
	}

	Pos2 lastPos = aiControl.lastPos;
	aiControl.lastPos = trans.position;

	if (PosDistance(lastPos, trans.position) < 10.f && RandomBool::get())
	{
		if (RandomBool::get(0.65))
		{
			controller.setWantToRotate(Controller::RotationDirection::NONE);
			controller.setWantToMove(RandomBool::get() ? Controller::MoveDirection::FORWARD : Controller::MoveDirection::BACKWARD);
		}
		else
		{
			controller.setWantToMove(Controller::MoveDirection::NONE);
			controller.setWantToRotate(RandomBool::get() ? Controller::RotationDirection::LEFT : Controller::RotationDirection::RIGHT);
		}
		return;
	}

	const float MAX_WALL_DISTANCE = std::max(tankProperty.width * 1.5f, tankProperty.height * 1.5f);

	auto nearbywallviews = world.getEntitiesWith<TagWall, WallProperty, Transform>() |
		std::views::filter([&trans, MAX_WALL_DISTANCE](Entity entity) {
		auto* wallproperty = entity.tryGetComponent<WallProperty>();
		auto* walltrans = entity.tryGetComponent<Transform>();
		if (!wallproperty || !walltrans) return false;
		return (PosDistance(trans.position, walltrans->position) < MAX_WALL_DISTANCE);
			});

	std::vector<Entity> nearbyWalls(nearbywallviews.begin(), nearbywallviews.end());

	auto& targettrans = target.getComponent<Transform>();

	Vec2 direction_Target = (targettrans.position.toVec2() - trans.position.toVec2()).normalize();
	Vec2 direction_Wall = nearbyWalls.empty() ?
		direction_Target :
		std::accumulate(nearbyWalls.begin(), nearbyWalls.end(), Vec2(0.f, 0.f),
			[&trans, MAX_WALL_DISTANCE](Vec2 v, Entity& e) {
				auto& wallproperty = e.getComponent<WallProperty>();
				auto& walltrans = e.getComponent<Transform>();
				Vec2 away_dir = (trans.position.toVec2() - walltrans.position.toVec2()).normalize();

				float dis = PosDistance(trans.position, walltrans.position);
				float strength = 1.0f - (dis / MAX_WALL_DISTANCE);
				strength = strength * strength;
				if (wallproperty.type == WallProperty::WallType::BRICK)
					strength = strength / 5.0f;
				return v + away_dir * strength;
			}
	).normalize();

	float distanceToTarget = PosDistance(trans.position, targettrans.position);
	float to_target_dir = std::atan2(direction_Target.y, direction_Target.x) * 180.0 / M_PI;
	float to_target_delta = normalizeDegrees(to_target_dir - trans.rotation);

	// 情况1：正在瞄准目标 - 减少墙的权重，允许靠近墙瞄准
	if (std::abs(to_target_delta) < 30.0f && distanceToTarget < 250.0f) {
		wall_factor = 0.5f;  // 正在瞄准，不那么怕墙
	}
	// 情况2：离目标很远 - 正常避墙
	else if (distanceToTarget > 400.f) {
		wall_factor = 1.3f;
	}
	// 情况3：离目标很近但没瞄准 - 需要调整位置，允许靠近墙
	else if (distanceToTarget < 200.0f) {
		wall_factor = 0.75f;
	}
	// 情况4：默认情况
	else {
		wall_factor = 1.0f;
	}

	Vec2 direction = (direction_Target * target_factor + direction_Wall * wall_factor).normalize();

	float move_targetrot = std::atan2(direction.y, direction.x) * 180.0 / M_PI;

	trans.rotation = normalizeDegrees(trans.rotation);
	move_targetrot = normalizeDegrees(move_targetrot);

	float move_delta = normalizeDegrees(move_targetrot - trans.rotation);

	if (std::abs(move_delta) < 0.01)
		controller.setWantToRotate(Controller::RotationDirection::NONE);
	else
	{
		if (move_delta < 0.f)
			controller.setWantToRotate(Controller::RotationDirection::LEFT);
		else
			controller.setWantToRotate(Controller::RotationDirection::RIGHT);
	}

	auto& movement = AITank.getComponent<Movement>();
	float rot_radius = movement.maxMoveSpeed / (360.f / movement.maxRotationSpeed);

	if (PosDistance(trans.position, targettrans.position) > rot_radius)
		controller.setWantToMove(std::abs(move_delta) <= 90.f ? Controller::MoveDirection::FORWARD : Controller::MoveDirection::BACKWARD);
	else
		controller.setWantToMove(Controller::MoveDirection::NONE);

	controller.setWantToFire(std::abs(to_target_delta) < 15.f);

	if (!controller.wantToFire)
	{
		if (auto* weapon = AITank.tryGetComponent<Weapon>())
		{
			if (weapon->maxBullets <= 2)
				controller.setWantToFire(true);
			else if (weapon->maxBullets - weapon->currentBullets > 2)
				controller.setWantToFire(true);
		}
	}
}

void AIInputSystem::drawDebugInfo(Entity AITank)
{
	static auto getlines = [](const Pos2& pos, const Vec2& dir, float length = 10.f)->Line2
	{
		Pos2 start = pos;
		Pos2 end = pos + Pos2(dir.normalize() * length);
		Line2 result(start, end);
		return result;

	};

	auto& world = getWorld();

	auto* rendersystem = world.getSystem<RenderSystem>();
	if (!rendersystem)
		return;

	float target_factor = 1.f;
	float wall_factor = 1.f;

	std::vector<Line2> debuglines;

	auto& aiControl = world.getComponent<AIControl>(AITank);
	auto& tankProperty = world.getComponent<TankProperty>(AITank);
	auto& trans = world.getComponent<Transform>(AITank);

	auto alltargets = world.getEntitiesWith<TagTank, TankProperty, Transform>() |
		std::views::filter([&AITank](Entity entity) {
		auto* tankProperty = entity.tryGetComponent<TankProperty>();
		if (!tankProperty) return false;
		return entity != AITank;
			});

	if (alltargets.empty())
		return;

	auto target = *std::ranges::min_element(alltargets, {},
		[&trans](Entity& e) {
			if (auto* targettrans = e.tryGetComponent<Transform>())
				return PosDistance(trans.position, targettrans->position);
			return FLT_MAX;
		});

	if (!target.hasComponent<Transform>())
		return;

	const float MAX_WALL_DISTANCE = std::max(tankProperty.width * 1.5f, tankProperty.height * 1.5f);

	auto nearbywallviews = world.getEntitiesWith<TagWall, WallProperty, Transform>() |
		std::views::filter([&trans, MAX_WALL_DISTANCE](Entity entity) {
		auto* wallproperty = entity.tryGetComponent<WallProperty>();
		auto* walltrans = entity.tryGetComponent<Transform>();
		if (!wallproperty || !walltrans) return false;
		return (PosDistance(trans.position, walltrans->position) < MAX_WALL_DISTANCE);
			});

	std::vector<Entity> nearbyWalls(nearbywallviews.begin(), nearbywallviews.end());

	auto& targettrans = target.getComponent<Transform>();

	Vec2 direction_Target = (targettrans.position.toVec2() - trans.position.toVec2()).normalize();
	Vec2 direction_Wall = nearbyWalls.empty() ?
		direction_Target :
		std::accumulate(nearbyWalls.begin(), nearbyWalls.end(), Vec2(0.f, 0.f),
			[&](Vec2 v, Entity& e) {
				auto& wallproperty = e.getComponent<WallProperty>();
				auto& walltrans = e.getComponent<Transform>();
				Vec2 away_dir = (trans.position.toVec2() - walltrans.position.toVec2()).normalize();

				float dis = PosDistance(trans.position, walltrans.position);
				float strength = 1.0f - (dis / MAX_WALL_DISTANCE);
				strength = strength * strength;
				if (wallproperty.type == WallProperty::WallType::BRICK)
					strength = strength / 5.0f;

				debuglines.push_back(getlines(walltrans.position, away_dir, 20));

				return v + away_dir * strength;
			}
	).normalize();

	float distanceToTarget = PosDistance(trans.position, targettrans.position);
	float to_target_dir = std::atan2(direction_Target.y, direction_Target.x) * 180.0 / M_PI;
	float to_target_delta = normalizeDegrees(to_target_dir - trans.rotation);

	// 情况1：正在瞄准目标 - 减少墙的权重，允许靠近墙瞄准
	if (std::abs(to_target_delta) < 30.0f && distanceToTarget < 250.0f) {
		wall_factor = 0.5f;  // 正在瞄准，不那么怕墙
	}
	// 情况2：离目标很远 - 正常避墙
	else if (distanceToTarget > 400.f) {
		wall_factor = 1.3f;
	}
	// 情况3：离目标很近但没瞄准 - 需要调整位置，允许靠近墙
	else if (distanceToTarget < 200.0f) {
		wall_factor = 0.75f;
	}
	// 情况4：默认情况
	else {
		wall_factor = 1.0f;
	}

	Vec2 direction = (direction_Target * target_factor + direction_Wall * wall_factor).normalize();

	debuglines.push_back(getlines(trans.position, direction_Target, 40));
	debuglines.push_back(getlines(trans.position, direction_Wall, 40));
	debuglines.push_back(getlines(trans.position, direction, 40));

	for (auto& line : debuglines)
		rendersystem->pushDebugLine(line);
}
