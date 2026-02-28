#pragma once

#include "stdafx.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Core/System.h"
#include "Helper/TripleBuffer.h"
#include "ECS/Event/SyncEventDef.h"

class ClientSyncSystem : public System
{
public:
	ClientSyncSystem();
	virtual ~ClientSyncSystem();
	virtual void preUpdate(float deltaTime) override;

	void InputGameState(const GameState& newstate);
	void InputEvent(const SyncEvent& e);

private:
	void ProcessSyncEvents(std::vector<SyncEvent>& syncevents);
	Entity findEntityBySyncId(const SyncID& syncId);

private:
	void SyncGameState(const GameState& allstate);

	void SyncTanks(const std::vector<TankState>& tankStates, std::unordered_map<SyncID, Entity>& clienttankmap, uint64_t server_timestamp);
	void SyncBullet(const std::vector<BulletState>& bulletStates, std::unordered_map<SyncID, Entity>& clientbulletmap, uint64_t server_timestamp);
	void SyncWall(const std::vector<WallState>& wallStates, std::unordered_map<SyncID, Entity>& clientwallmap);
	void SyncProp(const std::vector<PropState>& propStates, std::unordered_map<SyncID, Entity>& clientpropmap);

	void handleSyncTankFromServer(const TankState& state, Entity entity, uint64_t server_timestamp);
	void handleCreateClientTank(const TankState& state, uint64_t server_timestamp);

	void handleSyncBulletFromServer(const BulletState& state, Entity entity, uint64_t server_timestamp);
	void handleCreateClientBullet(const BulletState& state, uint64_t server_timestamp);

	void handleSyncWallFromServer(const WallState& state, Entity entity);
	void handleCreateClientWall(const WallState& state);

	void handleSyncPropFromServer(const PropState& state, Entity entity);
	void handleCreateClientProp(const PropState& state);


private:
	std::shared_ptr<TripleBuffer<GameState>> _gamestate_tripbuffer;
	SafeArray<SyncEvent> _syncEvents;
};