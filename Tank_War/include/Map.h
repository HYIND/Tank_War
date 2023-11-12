#pragma once
#include "header.h"
#include "Tank.h"
#include "Game_Component.h"
#include "Prop.h"
#include "ResourceManager.h"

struct Init_info {
	int Tank_id = 0;							//编号
	double x = 0;								//初始x
	double y = 0;								//初始y
	double rotate = 0;							//初始方向
	TankStyle tank_style = TankStyle::DEFAULT;	//初始样式
	bool isalive = true;

	Init_info(int Tank_id, double x, double y, double rotate, TankStyle tank_style = TankStyle::DEFAULT, bool isalive = true)
		:Tank_id(Tank_id), x(x), y(y), rotate(rotate), tank_style(tank_style), isalive(isalive) {}
};

class Map
{
public:
	int map_id;
	int user_limited = 0;

	vector<Init_info> Init_Location;
	ID2D1Bitmap* BK_pBitmap;
	vector<Game_Component*> Component_info;

public:
	Map();
	Map(int id, int user_limited);
	~Map();
	Map& operator=(Map& map);
	void DrawMap();
	void DrawMap(function<void()> callback);
	void Clear();
	void Save(char*& buf, int& len);
	bool Load(const char* buf, const int len);
};

extern int Cur_Map_id;
extern map<int, Map*> Map_list;

void Init_Map();