#pragma once
//#include "framework.h"
//#include "Scene.h"

//// WinSocket
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")
////#pragma comment(lib,"Kr")
// 
//#pragma comment(lib,"dxguid.lib")

#include "header.h"
#include "Style.h"
#include "D2D.h"
#pragma comment(lib,"Msimg32.lib")
using namespace std;

extern ID2D1SolidColorBrush* pbullet_Brush;

class Tank;

enum Direction{ DOWN, LEFT, UP, RIGHT };
class bullet
{
public:
	int locationX;
	int locationY;
	int direction;
	BulletStyle bullet_style = BulletStyle::DEFAULT;

	int width = 2;
	int height = 2;
	int speed;

	Tank* owner = NULL;
	bullet* next = NULL;
	bullet* last = NULL;

	bullet() {};
	bullet(int locationX, int locationY, int direction, int speed, Tank* owner = NULL, bullet* last = NULL) {
		this->locationX = locationX;
		this->locationY = locationY;
		this->direction = direction;
		this->speed = speed;
		this->owner = owner;
		this->last = last;
	}

	void Drawbullet();
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
	//bool isregister = false;

	bullet* bullet_head = NULL;

public:
	TankStyle tank_style = TankStyle::DEFAULT;

	Tank();
	Tank(Tank* t);
	Tank(int width, int height);
	Tank(int X, int Y, int width, int height, int direction, bool alive = true);
	Tank(int X, int Y, int width, int height, int direction, bool alive, bool isregister);
	void InitTank(int X, int Y, int width, int height, int direction);
	void InitTank(int X, int Y, int direction);
	void InitTank(int width, int height);
	void DrawTank(ID2D1HwndRenderTarget* pRenderTarget = ::pRenderTarget);
	//void Tank1_Move(RECT& rect);
	//void Tank2_Move(RECT& rect);
	void Tank_Move(Direction direction);
	void Addbullet();
	bool crash();

	friend class bullet;
};

//struct tank_info {
//	Tank* cur = NULL;
//	Tank* Init = NULL;
//	tank_info(Tank* cur) {
//		this->cur = cur;
//	}
//	void Get_Initinfo() {
//		this->Init = new Tank(this->cur);
//	}
//	void Returnto_Initinfo() {
//		(*(this->cur)).InitTank(Init->locationX, Init->locationY, Init->width, Init->height, Init->direction);
//		this->cur->isalive = this->Init->isalive;
//		this->cur->bullet_head = NULL;
//	}
//};

void Get_Initinfo();
void Return_Tankinfo();
void Init_all();

void send_location(Tank* tank);
void send_bullet(bullet* cur);
void send_destroy(bullet* bullet);
void Refresh_opTank(char buf[]);
void Refresh_opbullet(string& re);
void destory_bulletinfo();
void my_destroy();
void op_destory();
