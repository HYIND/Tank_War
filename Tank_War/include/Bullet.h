#pragma once

#include "header.h"
#include "Style.h"
#include "Object.h"
#include "Tank.h"
#include "Scene.h"

class Tank;

class bullet :public RoundObject
{
public:
	BulletStyle bullet_style = BulletStyle::DEFAULT;

	int speed;
	bool isBounce = false;	//反弹中的标志，子弹在障碍物边界反复回弹
	int last_bounceId = 0;
	bool isdestroy = false;
	int BounceCount = 0;

	Tank* owner = NULL;
	bullet* next = NULL;
	bullet* last = NULL;


	bullet() {};
	bullet(double x, double y, double rotate, BulletStyle bulletstyle = BulletStyle::DEFAULT, Tank* owner = NULL, bullet* last = NULL)
		:owner(owner), last(last), bullet_style(bulletstyle)
	{
		Style_info* style_info = Get_Parameter_byStyle(bulletstyle);
		this->location.x = x;
		this->location.y = y;
		this->radius = style_info->width;
		this->rotate = rotate;
		this->speed = style_info->speed;
	}

	Style_info* Get_Parameter_byStyle(BulletStyle bulletstyle);
	void Drawbullet();
	bool destroy();
};