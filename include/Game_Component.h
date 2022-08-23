#pragma once

#include "header.h"

using namespace std;

#define BRICK_WIDTH 56
#define BRICK_HEIGHT 56
#define IRON_WIDTH 56
#define IRON_HEIGHT 56

class Game_Component
{
	friend class Room_Process;
protected:
	int locationX;
	int locationY;
	int width;
	int height;
	int health = 100;
public:
	int id;
	Game_Component(int x, int y, int width, int height, int id, int health = 100)
		:locationX(x), locationY(y), width(width), height(height), id(id), health(health){}
	virtual ~Game_Component() {};
};

class Brick_Wall :public Game_Component
{
public:
	Brick_Wall(int x, int y, int id, int health = 60)
		:Game_Component(x, y, BRICK_WIDTH, BRICK_HEIGHT, id, health) {};
};

class Iron_Wall :public Game_Component
{
public:
	Iron_Wall(int x, int y, int id, int health = 99999999)
		:Game_Component(x, y, IRON_WIDTH, IRON_HEIGHT, id, health) {};
};