#pragma once

#include "NetworkMessageSender.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Core/System.h"
#include "Helper/TripleBuffer.h"
#include "GameDataDef.h"
#include "stdafx.h"
#include "ECS/Event/SyncEventDef.h"

class ServerSyncSystem : public System
{
public:
	ServerSyncSystem();
	virtual ~ServerSyncSystem();

	virtual void onAttach(World& world) override;
	virtual void postUpdate(float fixedDeltaTime) override;

	void SetNetworkMessageSender(std::shared_ptr<NetworkMessageSender>& sender);

	void SetSyncFps(float fps);
	float GetSyncFps();

private:
	void BroadCaseGameState();
	void BroadCaseEvent(const SyncEvent& event);

	void handleSyncTank(GameState& allstate, Entity entity);
	void handleSyncBullet(GameState& allstate, Entity entity);
	void handleSyncWall(GameState& allstate, Entity entity);
	void handleSyncProp(GameState& allstate, Entity entity);

	GameState& GetGameState();

private:
	std::shared_ptr<TripleBuffer<GameState>> _gamestate_tripbuffer;

	std::shared_ptr<NetworkMessageSender> _sender;

	float _update_fps;

	float _deltatime_Accumulator;
	float _update_deltatime_ms;
};