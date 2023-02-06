#pragma once

#include "header.h"
#include "Style.h"
#include "D2D.h"
#pragma comment(lib,"Msimg32.lib")
using namespace std;

extern ID2D1SolidColorBrush* pRed_Brush;

class Tank;

class bullet
{
public:
	double locationX;
	double locationY;
	double rotate = 0;
	BulletStyle bullet_style = BulletStyle::DEFAULT;

	int width = 2;
	int height = 2;
	int speed;
	bool isBounce = false;	//反弹中的标志，子弹在障碍物边界反复回弹
	int last_bounceId = 0;
	bool isdestroy = false;
	int BounceCount = 0;

	Tank* owner = NULL;
	bullet* next = NULL;
	bullet* last = NULL;

	bullet() {};
	bullet(double locationX, double locationY, double rotate, BulletStyle bulletstyle = BulletStyle::DEFAULT, Tank* owner = NULL, bullet* last = NULL)
		:locationX(locationX), locationY(locationY), rotate(rotate), owner(owner), last(last), bullet_style(bulletstyle)
	{
		Set_Parameter_byStyle(bulletstyle);
	}

	void Set_Parameter_byStyle(BulletStyle bulletstyle);
	void Drawbullet();
	bool destroy();
};

class Tank
{
public:
	double locationX = 0;
	double locationY = 0;
	int width = 0;
	int height = 0;
	double rotate = 0;
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

	friend class bullet;

};

//void destory_bulletinfo();
