#pragma once
#include "header.h"
#include "Style.h"
#include "Tank.h"

struct Init_info {
	int Tank_id = 0;			//编号
	int x = 0;					//初始x
	int y = 0;					//初始y
	int direction = UP;			//初始方向
	TankStyle tank_style = TankStyle::DEFAULT;	//初始样式
	bool isalive = true;

	Init_info(int Tank_id, int x, int y, int direction, TankStyle tank_style = TankStyle::DEFAULT, bool isalive = true)
		:Tank_id(Tank_id), x(x), y(y), direction(direction), tank_style(tank_style), isalive(isalive) {}
};

class Map
{
public:
	int map_id;
	int user_limited = 0;
	vector<Init_info> Init_Location;

public:
	Map() {}
	Map(int id, int user_limited) :map_id(id), user_limited(user_limited) {}
};

extern map<int, Map> Map_list;

void Init_Map();