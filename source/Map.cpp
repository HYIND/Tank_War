#include "Map.h"

map<int, Map> Map_list;

void Init_Map_Zero()
{
    Map Default_Map(0, 2);
	Default_Map.Init_Location.emplace_back(1, 50, 280, UP, TankStyle::DEFAULT);
	Default_Map.Init_Location.emplace_back(2, 1134, 280, UP, TankStyle::DEFAULT);

	int brick_count = 1;
	int icon_count = 1;

	//for (int i = 0; i < 10; i++)
	//{
	//	Brick_Wall brick(250 + i * BRICK_WIDTH, 150, brick_count);
	//	Default_Map.Brick_info.emplace_back(brick);
	//	brick_count++;
	//}

	//for (int i = 0; i < 10; i++)
	//{
	//	Brick_Wall brick(250 + i * 56, 400, brick_count);
	//	Default_Map.Brick_info.emplace_back(brick);
	//	brick_count++;
	//}

	for (int i = 0; i < 12; i++)
	{
		if (i == 5 || i == 6)
		{
			Brick_Wall brick(280 + i * IRON_WIDTH, 110, brick_count);
			Default_Map.Brick_info.emplace_back(brick);
			brick_count++;
			continue;
		}
		Iron_Wall iron(280 + i * IRON_WIDTH, 110, icon_count);
		Default_Map.Iron_info.emplace_back(iron);
		icon_count++;
	}
	for (int i = 0; i < 12; i++)
	{
		if (i == 5 || i == 6)
		{
			Brick_Wall brick(280 + i * IRON_WIDTH, 110 + 7 * IRON_HEIGHT, brick_count);
			Default_Map.Brick_info.emplace_back(brick);
			brick_count++;
			continue;
		}
		Iron_Wall iron(280 + i * IRON_WIDTH, 110 + 7 * IRON_HEIGHT, icon_count);
		Default_Map.Iron_info.emplace_back(iron);
		icon_count++;
	}
	for (int i = 1; i < 7; i++)
	{
		if (i == 3 || i == 4)
		{
			Brick_Wall brick(280, 110 + i * IRON_HEIGHT, brick_count);
			Default_Map.Brick_info.emplace_back(brick);
			brick_count++;
			continue;
		}
		Iron_Wall iron(280, 110 + i * IRON_HEIGHT, icon_count);
		Default_Map.Iron_info.emplace_back(iron);
		icon_count++;
	}
	for (int i = 1; i < 7; i++)
	{
		if (i == 3 || i == 4)
		{
			Brick_Wall brick(280 + 11 * IRON_WIDTH, 110 + i * IRON_HEIGHT, brick_count);
			Default_Map.Brick_info.emplace_back(brick);
			brick_count++;
			continue;
		}
		Iron_Wall iron(280 + 11 * IRON_WIDTH, 110 + i * IRON_HEIGHT, icon_count);
		Default_Map.Iron_info.emplace_back(iron);
		icon_count++;
	}
	for (int i = 5; i < 7; i++)
	{
		for (int j = 3; j < 5; j++)
		{
			Iron_Wall iron(280 + i * IRON_WIDTH, 110 + j * IRON_HEIGHT, icon_count);
			Default_Map.Iron_info.emplace_back(iron);
			icon_count++;
		}
	}
	Map_list[Default_Map.map_id] = Default_Map;
}

void Init_Map()
{
    Init_Map_Zero();
}