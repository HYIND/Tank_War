#include "Map.h"

map<int, Map> Map_list;

void Init_Map_Zero()
{
    Map Default_Map(0, 2);
    Default_Map.Init_Location.emplace_back(1, 50, 300, UP, TankStyle::DEFAULT);
    Default_Map.Init_Location.emplace_back(2, 500, 300, UP, TankStyle::DEFAULT);

	int brick_count = 1;
	for (int i = 0; i < 10; i++)
	{
		Brick_Wall brick(100 + i * 50, 200, brick_count);
		Default_Map.Brick_info.emplace_back(brick);
		brick_count++;
	}
	int icon_count = 1;
	for (int i = 0; i < 10; i++)
	{
		Iron_Wall iron(400, 200 + i * 50, icon_count);
		Default_Map.Iron_info.emplace_back(iron);
		icon_count++;
	}
    
    Map_list[Default_Map.map_id] = Default_Map;
}

void Init_Map()
{
    Init_Map_Zero();
}