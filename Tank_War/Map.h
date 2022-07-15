#pragma once
#include "header.h"
#include "Tank.h"
#include "Game_Component.h"
#include "D2D.h"

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

	ID2D1Bitmap* BK_pBitmap;
	vector<Brick_Wall> Brick_info;
	vector<Iron_Wall> Iron_info;

public:
	Map() {}
	Map(int id, int user_limited) :map_id(id), user_limited(user_limited) {}
	void DrawMap();
};

extern int Cur_Map_id;
extern map<int, Map> Map_list;

void Init_Map();