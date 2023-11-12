#pragma once

#include "header.h"
#include "Object.h"
#include "ResourceManager.h"

using namespace std;

#define BRICK_WIDTH 56
#define BRICK_HEIGHT 56
#define IRON_WIDTH 56
#define IRON_HEIGHT 56

enum class component_type :int {
	DEFAULT = 0,
	BRICK, IRON,				//墙体
	PROP_DEFAULT,
	AIDKIT						//道具
};

struct Com_Style_info
{
	int width;
	int height;
	int health;
	ID2D1Bitmap* Bitmap;
	Com_Style_info(int width, int height, int health, ID2D1Bitmap* pBitmap) :
		width(width), height(height), health(health), Bitmap(pBitmap) {};
};

extern map<component_type, Com_Style_info*> com_info;

class Game_Component :public RectObject
{
protected:
	int health = 100;
	ID2D1Bitmap* Bitmap = NULL;
public:
	component_type type = component_type::DEFAULT;
	int id;
	Game_Component(double x, double y, int width, int height, int id, int health = 100, ID2D1Bitmap* Bitmap = NULL, component_type type = component_type::DEFAULT)
		:RectObject(x, y, width, height), id(id), health(health), Bitmap(Bitmap), type(type) {}
	int get_health();
	void set_health(int health);
	int add_health(int add);
	int reduce_health(int reduce);
	virtual void Draw();
	virtual ~Game_Component() {};
};

class Brick_Wall :public Game_Component
{
public:
	Brick_Wall(double x, double y, int id, int health = 60, ID2D1Bitmap* Bitmap = ResFactory->GetBitMapRes(ResName::brickWall))
		:Game_Component(x, y, BRICK_WIDTH, BRICK_HEIGHT, id, health, Bitmap, component_type::BRICK) {};
	using Game_Component::Draw;
	~Brick_Wall() {};
};

class Iron_Wall :public Game_Component
{
public:
	Iron_Wall(double x, double y, int id)
		:Game_Component(x, y, IRON_WIDTH, IRON_HEIGHT, id, 99999999, ResFactory->GetBitMapRes(ResName::ironWall), component_type::IRON) {};
	using Game_Component::Draw;
	~Iron_Wall() {};
};


void Init_ComponentMap();