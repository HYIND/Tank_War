#pragma once
#include "framework.h"
#include <vector>
using namespace std;

class Tank;

enum { UP, DOWN, LEFT, RIGHT };
class bullet
{
public:
	int locationX;
	int locationY;
	int width = 2;
	int height = 2;
	int direction;
	int speed;

	Tank* owner = NULL;
	bullet* next = NULL;
	bullet* last = NULL;

	bullet(){};
	bullet(int locationX, int locationY, int direction, int speed, Tank* owner = NULL, bullet* last = NULL) {
		this->locationX = locationX;
		this->locationY = locationY;
		this->direction = direction;
		this->speed = speed;
		this->owner = owner;
		this->last = last;
	}

	void Drawbullet(HPEN& Pen, HBRUSH& Brush);
	void Move(RECT rect);
	bool crash(RECT rect, int direction);
	void destroy();
};

class Tank
{
public:
	int locationX = 0;
	int locationY = 0;
	int width = 0;
	int height = 0;
	int direction = UP;

	bool isalive = true;
	bool isregister = false;
	bullet* bullet_head = NULL;

	Tank();
	Tank(Tank* t);
	Tank(int width, int height);
	Tank(int X, int Y, int width, int height, int direction);
	Tank(int X, int Y, int width, int height, int direction, bool alive, bool isregister);
	void InitTank(int X, int Y, int width, int height, int direction);
	void InitTank(int X, int Y, int direction);
	void InitTank(int width, int height);
	void DrawTank(RECT& rect, HBITMAP& hbitmap, BITMAP& bm);
	void Tank1_Move(RECT& rect);
	void Tank2_Move(RECT& rect);
	void Addbullet();
	bool crash();

private:

};

struct tank_info {
	Tank* cur = NULL;
	Tank* Init = NULL;
	tank_info(Tank* cur) {
		this->cur = cur;
	}
	void Get_Initinfo() {
		this->Init = new Tank(this->cur);
	}
	void Returnto_Initinfo() {
		(*(this->cur)).InitTank(Init->locationX, Init->locationY, Init->width, Init->height, Init->direction);
		this->cur->isalive = this->Init->isalive;
		this->cur->bullet_head = NULL;
	}
};

void Get_Initinfo();
void Return_Tankinfo();
void Init_all();
