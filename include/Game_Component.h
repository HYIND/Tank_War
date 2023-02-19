#pragma once

#include "header.h"
#include "Object.h"

using namespace std;

#define BRICK_WIDTH 56
#define BRICK_HEIGHT 56
#define IRON_WIDTH 56
#define IRON_HEIGHT 56

enum class component_type : int
{
	DEFAULT = 0,
	BRICK,
	IRON, // 墙体
	PROP_DEFAULT,
	AIDKIT // 道具
};

class Game_Component :public RectObject
{
protected:
	int health = 100;

public:
	component_type type = component_type::DEFAULT;
	int id;
	Game_Component(int x, int y, int width, int height, int id, int health = 100, component_type type = component_type::DEFAULT)
		: RectObject(x, y, width, height), id(id), health(health), type(type) {}

	int get_health();
	void set_health(int health);
	int add_health(int add);
	int reduce_health(int reduce);
	virtual ~Game_Component(){};
};

class Brick_Wall : public Game_Component
{
public:
	Brick_Wall(int x, int y, int id, int health = 60)
		: Game_Component(x, y, BRICK_WIDTH, BRICK_HEIGHT, id, health, component_type::BRICK){};
};

class Iron_Wall : public Game_Component
{
public:
	Iron_Wall(int x, int y, int id, int health = 99999999)
		: Game_Component(x, y, IRON_WIDTH, IRON_HEIGHT, id, health, component_type::IRON){};
};