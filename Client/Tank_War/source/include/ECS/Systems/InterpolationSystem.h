#pragma once

#include "ECS/Core/System.h"
#include "ECS/Components/AllComponent.h"

class InterpolationSystem : public System {
public:
	void update(float dt) override;

private:
	std::pair<Interpolation::Snapshot*, Interpolation::Snapshot*>
		findSnapshotsForTime(Interpolation& interp, uint64_t targetTime);

	float calculateInterpolationAlpha(const Interpolation::Snapshot& older,
		const Interpolation::Snapshot& newer,
		uint64_t targetTime);

	void interpolateTransform(Transform& transform,
		const Interpolation::Snapshot& older,
		const Interpolation::Snapshot& newer,
		float alpha);

	void cleanupOldSnapshots(Interpolation& interp, uint64_t targetTime);
};