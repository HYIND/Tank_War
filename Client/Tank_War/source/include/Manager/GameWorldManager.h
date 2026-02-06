#pragma once

#include "Manager/MapManager.h"
#include "ECS/Core/World.h"
#include <memory>
#include <thread>
#include "ECS/Systems/ClientStateSyncStstem.h"

enum class GameMode
{
	RunGame,
	MapEdit
};

class GameWorldManager
{
public:
	static GameWorldManager* Instance();

	std::shared_ptr<World> GetGameWorld();

	void InitGameWorld(GameMode mode, MapID mapid);
	void InitOnlineGameWorld();

	void RunWorld();
	void WorldLoop();
	void StopWorld();

	std::shared_ptr<std::thread> GetWorldThread();


	void SyncFromServerState(const json& js);
private:
	GameWorldManager();


private:
	std::shared_ptr<World> _world;
	std::shared_ptr<std::thread> _worldThread;
	bool stop;
};
