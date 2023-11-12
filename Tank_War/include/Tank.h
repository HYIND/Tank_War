#pragma once

#include "Style.h"
#include "D2D.h"
#include "Object.h"
#include "Bullet.h"
#pragma comment(lib,"Msimg32.lib")
using namespace std;

class bullet;

class Tank :public RectObject
{
public:
	TankStyle tank_style = TankStyle::DEFAULT;

	bool isalive = true;
	int health = 100;
	int speed = 3;
	int bullet_count = 0;
	int bullet_limited = 5;

	// 记录上次发射子弹时间，处理发射间隔
	int bullet_last = 0;
	int bullet_now = 0;


	bullet* bullet_head = NULL;

public:

	Tank() {}
	Tank(const Tank& t) {}
	Tank(int width, int height);
	Tank(double X, double Y, int width, int height, double rotate, int speed, bool alive = true);
	void Set_Parameter_byStyle(TankStyle tankstyle);
	void DrawTankHP(ID2D1HwndRenderTarget* pRenderTarget = ::pRenderTarget);
	void DrawTank(ID2D1HwndRenderTarget* pRenderTarget = ::pRenderTarget);
	void Addbullet(BulletStyle bulletstyle);
	void destory();

	friend class bullet;
};

//void destory_bulletinfo();
