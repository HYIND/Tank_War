#pragma once

#include "header.h"

using namespace std;

#define BRICK_WIDTH 56
#define BRICK_HEIGHT 56
#define IRON_WIDTH 56
#define IRON_HEIGHT 56

extern ID2D1Bitmap* brick_wall_pBitmap;
extern ID2D1Bitmap* iron_wall_pBitmap;

class Game_Component
{
	friend class Game;
protected:
	int locationX;
	int locationY;
	int width;
	int height;
	int health = 100;
	ID2D1Bitmap* Bitmap = NULL;
public:
	int id;
	Game_Component(int x, int y, int width, int height, int id, int health = 100, ID2D1Bitmap* Bitmap = NULL)
		:locationX(x), locationY(y), width(width), height(height), id(id), health(health), Bitmap(Bitmap) {}
	virtual void Draw();
	virtual ~Game_Component() {};
};

class Brick_Wall :public Game_Component
{
public:
	Brick_Wall(int x, int y, int id, int health = 60, ID2D1Bitmap* Bitmap = brick_wall_pBitmap)
		:Game_Component(x, y, BRICK_WIDTH, BRICK_HEIGHT, id, health, brick_wall_pBitmap) {};
	using Game_Component::Draw;
};

class Iron_Wall :public Game_Component
{
public:
	Iron_Wall(int x, int y, int id, int health = 99999999, ID2D1Bitmap* Bitmap = iron_wall_pBitmap)
		:Game_Component(x, y, IRON_WIDTH, IRON_HEIGHT, id, health, iron_wall_pBitmap) {};
	using Game_Component::Draw;
};


void Init_Component_Resource();