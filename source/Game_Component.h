#include "header.h"

using namespace std;

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
		:Game_Component(x, y, 50, 50, id, health) {};
};

class Iron_Wall :public Game_Component
{
public:
	Iron_Wall(int x, int y, int id, int health = 99999999)
		:Game_Component(x, y, 50, 50, id, health) {};
};