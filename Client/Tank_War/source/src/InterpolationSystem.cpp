#include "ECS/Systems/InterpolationSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Core/Entity.h"
#include "ECS/Core/System.h"

#include "ECS/Event/ECSEventDef.h"
#include "ECS/Components/AllComponent.h"

#include "Manager/UserInfoManager.h"

void InterpolationSystem::update(float dt)
{
	auto entities = m_world->getEntitiesWith<Interpolation, Transform>();

	for (auto entity : entities) {
		auto& interp = entity.getComponent<Interpolation>();
		auto& transform = entity.getComponent<Transform>();

		if (auto* tankproperty = entity.tryGetComponent<TankProperty>())
		{
			if (UserInfoManager::Instance()->isMyToken(tankproperty->playerId))
			{
				cleanupOldSnapshots(interp, Tool::GetTimestampMilliseconds());
				continue;
			}
		}

		if (!interp.enabled || interp.snapshots.size() < 2) {
			// 没有足够数据，直接使用最新快照
			if (!interp.snapshots.empty()) {
				transform.position = interp.snapshots.back().position;
				transform.rotation = interp.snapshots.back().rotation;
			}
			continue;
		}

		// 更新渲染时间线
		interp.currentRenderTime = Tool::GetTimestampMilliseconds();

		// 计算目标插值时间
		uint64_t targetTime = interp.currentRenderTime - interp.interpolationDelay;

		// 清理过旧的快照
		cleanupOldSnapshots(interp, targetTime);

		// 找到包围目标时间的两个快照
		auto [older, newer] = findSnapshotsForTime(interp, targetTime);

		if (older && newer && older != newer) {
			// 计算插值因子
			float alpha = calculateInterpolationAlpha(*older, *newer, targetTime);

			// 执行插值
			interpolateTransform(transform, *older, *newer, alpha);

			// 更新插值组件中的渲染状态
			//updateInterpolationState(interp, transform, alpha);

			// 计算延迟信息（调试用）
			//interp.latestSnapshotDelay = getCurrentTime() - newer->receiveTime;
		}
	}
}

std::pair<Interpolation::Snapshot*, Interpolation::Snapshot*>
InterpolationSystem::findSnapshotsForTime(Interpolation& interp, uint64_t targetTime) {
	if (interp.snapshots.size() < 2) return { nullptr, nullptr };

	Interpolation::Snapshot* older = nullptr;
	Interpolation::Snapshot* newer = nullptr;

	for (size_t i = 0; i < interp.snapshots.size() - 1; i++) {
		if (interp.snapshots[i].receiveTime <= targetTime &&
			interp.snapshots[i + 1].receiveTime >= targetTime) {
			return { &interp.snapshots[i], &interp.snapshots[i + 1] };
		}
	}

	// 1. target时间在所有快照之前 - 使用前两个
	if (targetTime < interp.snapshots.front().receiveTime) {
		return { &interp.snapshots[0], &interp.snapshots[1] };
	}

	// 2. target时间在所有快照之后 - 使用后两个
	if (targetTime > interp.snapshots.back().receiveTime) {
		size_t last = interp.snapshots.size() - 1;
		return { &interp.snapshots[last - 1], &interp.snapshots[last] };
	}

	// 3. 理论不应该到这里，安全返回最新两个
	size_t last = interp.snapshots.size() - 1;
	return { &interp.snapshots[last - 1], &interp.snapshots[last] };
}

float InterpolationSystem::calculateInterpolationAlpha(
	const Interpolation::Snapshot& older, const Interpolation::Snapshot& newer,
	uint64_t targetTime)
{
	if (newer.receiveTime == older.receiveTime) {
		return 0.5f;  // 取中间值
	}

	// 计算插值比例
	uint64_t timeRange = newer.receiveTime - older.receiveTime;
	uint64_t timeOffset = targetTime - older.receiveTime;

	return static_cast<float>(timeOffset) / static_cast<float>(timeRange);
}

void InterpolationSystem::interpolateTransform(Transform& transform,
	const Interpolation::Snapshot& older,
	const Interpolation::Snapshot& newer,
	float alpha) {
	// 位置插值（线性）
	transform.position.x = older.position.x + (newer.position.x - older.position.x) * alpha;
	transform.position.y = older.position.y + (newer.position.y - older.position.y) * alpha;

	// 旋转插值（处理角度环绕）
	float fromAngle = older.rotation;
	float toAngle = newer.rotation;

	// 找到最短的旋转方向
	float diff = toAngle - fromAngle;
	if (diff > 180.0f) diff -= 360.0f;
	if (diff < -180.0f) diff += 360.0f;

	transform.rotation = fromAngle + diff * alpha;

	// 归一化角度
	if (transform.rotation > 180.0f) transform.rotation -= 360.0f;
	if (transform.rotation < -180.0f) transform.rotation += 360.0f;
}

void InterpolationSystem::cleanupOldSnapshots(Interpolation& interp, uint64_t targetTime) {
	uint64_t oldestAllowed = targetTime - interp.interpolationDelay * 2;

	while (!interp.snapshots.size() > 3 &&
		interp.snapshots.front().receiveTime < oldestAllowed) {
		interp.snapshots.pop_front();
	}

	// 确保不超过最大数量
	while (interp.snapshots.size() > interp.maxSnapshots) {
		interp.snapshots.pop_front();
	}
}
