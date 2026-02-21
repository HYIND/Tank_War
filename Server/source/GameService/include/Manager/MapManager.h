#pragma once

#include "ECS/Components/TankVisual.h"
#include "ECS/Components/PropProperty.h"
#include "ECS/Components/WallProperty.h"
#include "Helper/math2d.h"
#include <vector>
#include <map>
#include <iostream>
#include <cstdint>

using MapID = uint32_t;

namespace MapBoundary
{
	inline int left = 0;
	inline int right = 1000;
	inline int top = 0;
	inline int bottom = 600;
}

struct BrithInfo
{
	Pos2 position = {0, 0};
	float rotation = 0;
	int width = 0;
	int height = 0;
	BrithInfo(Pos2 pos, float rotation, int width, int height)
		: position(pos), rotation(rotation), width(width), height(height)
	{
	}
};
struct TankBirthInfo : public BrithInfo
{
	TankVisual::VisualState visual;
	TankBirthInfo(Pos2 pos, float rotation, int width, int height, TankVisual::VisualState visual)
		: BrithInfo(pos, rotation, width, height), visual(visual)
	{
	}
};
struct AITankBirthInfo : public BrithInfo
{
	TankVisual::VisualState visual;
	AITankBirthInfo(Pos2 pos, float rotation, int width, int height, TankVisual::VisualState visual)
		: BrithInfo(pos, rotation, width, height), visual(visual)
	{
	}
};
struct PropBirthInfo : public BrithInfo
{
	PropProperty::PropType type;
	float duration = 5.f;
	PropBirthInfo(Pos2 pos, float rotation, int width, int height, PropProperty::PropType type)
		: BrithInfo(pos, rotation, width, height), type(type)
	{
	}
};
struct WallBirthInfo : public BrithInfo
{
	WallProperty::WallType type;
	int health = 100;
	WallBirthInfo(Pos2 pos, float rotation, int width, int height, int health, WallProperty::WallType type)
		: BrithInfo(pos, rotation, width, height), health(health), type(type)
	{
	}
};

struct MapInfo
{
	std::vector<TankBirthInfo> tankbirthinfos;
	std::vector<AITankBirthInfo> aitankbirthinfos;
	std::vector<PropBirthInfo> propbirthinfos;
	std::vector<WallBirthInfo> wallbirthinfos;
	std::string backGrounp_resname = "";
};

class MapManager
{
public:
	static MapManager *Instance();

	bool isMapExist(MapID id);
	MapInfo getMap(MapID id);

private:
	MapManager();

private:
	std::map<MapID, MapInfo> _maps;
};