#include "Map.h"

map<int, Map *> Map_list;

Map &Map::operator=(Map &other)
{
	this->map_id = other.map_id;
	this->user_limited = other.user_limited;
	this->Init_Location = other.Init_Location;
	this->Component_info.clear();
	for (auto &v : other.Component_info)
	{
		Game_Component *com_temp = v;
		Game_Component *pCom;
		bool dynamic_result = false;
		switch (com_temp->type)
		{
		case component_type::BRICK:
		{
			Brick_Wall *pbrick_temp = nullptr;
			if (pbrick_temp = dynamic_cast<Brick_Wall *>(com_temp))
			{
				dynamic_result = true;
				Brick_Wall *p = new Brick_Wall(*pbrick_temp);
				pCom = p;
			}
			break;
		}
		case component_type::IRON:
		{
			Iron_Wall *piron_temp = nullptr;
			if (piron_temp = dynamic_cast<Iron_Wall *>(com_temp))
			{
				dynamic_result = true;
				Iron_Wall *p = new Iron_Wall(*piron_temp);
				pCom = p;
			}
			break;
		}
		case component_type::AIDKIT:
		{
			Aid_kit *pAidkit_temp = nullptr;
			if (pAidkit_temp = dynamic_cast<Aid_kit *>(com_temp))
			{
				dynamic_result = true;
				Aid_kit *p = new Aid_kit(*pAidkit_temp);
				pCom = p;
			}
			break;
		}
		default:
			break;
		}
		if (dynamic_result)
			this->Component_info.emplace_back(pCom);
	}
	return *this;
}

void Init_Map_Zero()
{
	Map *Default_Map = new Map(0, 2);

	Default_Map->Init_Location.emplace_back(1, 50, 280, 0, TankStyle::DEFAULT);
	Default_Map->Init_Location.emplace_back(2, 1134, 280, 0, TankStyle::DEFAULT);

	int brick_count = 1;
	int icon_count = 1;

	// for (int i = 0; i < 10; i++)
	//{
	//	Brick_Wall* brick = new Brick_Wall(250 + i * BRICK_WIDTH, 150, brick_count);
	//	Default_Map.Brick_info.emplace_back(brick);
	//	brick_count++;
	// }

	// for (int i = 0; i < 10; i++)
	//{
	//	Brick_Wall* brick = new Brick_Wall(250 + i * 56, 400, brick_count);
	//	Default_Map.Brick_info.emplace_back(brick);
	//	brick_count++;
	// }

	for (int i = 0; i < 12; i++)
	{
		if (i == 5 || i == 6)
		{
			Brick_Wall *brick = new Brick_Wall(280 + i * IRON_WIDTH, 110, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component *)brick);
			brick_count++;
			continue;
		}
		Iron_Wall *iron = new Iron_Wall(280 + i * IRON_WIDTH, 110, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component *)iron);
		icon_count++;
	}
	for (int i = 0; i < 12; i++)
	{
		if (i == 5 || i == 6)
		{
			Brick_Wall *brick = new Brick_Wall(280 + i * IRON_WIDTH, 110 + 7 * IRON_HEIGHT, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component *)brick);
			brick_count++;
			continue;
		}
		Iron_Wall *iron = new Iron_Wall(280 + i * IRON_WIDTH, 110 + 7 * IRON_HEIGHT, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component *)iron);
		icon_count++;
	}
	for (int i = 1; i < 7; i++)
	{
		if (i == 3 || i == 4)
		{
			Brick_Wall *brick = new Brick_Wall(280, 110 + i * IRON_HEIGHT, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component *)brick);
			brick_count++;
			continue;
		}
		Iron_Wall *iron = new Iron_Wall(280, 110 + i * IRON_HEIGHT, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component *)iron);
		icon_count++;
	}
	for (int i = 1; i < 7; i++)
	{
		if (i == 3 || i == 4)
		{
			Brick_Wall *brick = new Brick_Wall(280 + 11 * IRON_WIDTH, 110 + i * IRON_HEIGHT, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component *)brick);
			brick_count++;
			continue;
		}
		Iron_Wall *iron = new Iron_Wall(280 + 11 * IRON_WIDTH, 110 + i * IRON_HEIGHT, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component *)iron);
		icon_count++;
	}
	for (int i = 5; i < 7; i++)
	{
		for (int j = 3; j < 5; j++)
		{
			Iron_Wall *iron = new Iron_Wall(280 + i * IRON_WIDTH, 110 + j * IRON_HEIGHT, icon_count);
			Default_Map->Component_info.emplace_back((Game_Component *)iron);
			icon_count++;
		}
	}
	Map_list[Default_Map->map_id] = Default_Map;
}

void Init_Map_Debug()
{
	Map *Debug_Map = new Map(-1, 2);

	Debug_Map->Init_Location.emplace_back(1, 50, 280, 0, TankStyle::DEFAULT);
	Debug_Map->Init_Location.emplace_back(2, 1134, 280, 0, TankStyle::DEFAULT);

	Map_list[Debug_Map->map_id] = Debug_Map;
}

void Init_Map()
{
	Init_Map_Zero();
	Init_Map_Debug();
}