#pragma once
#include "Manager/ResourceManager.h"
using namespace std;

enum class TankStyle :int { NONE = -1, DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };
enum class BulletStyle :int { NONE = -1, DEFAULT, BIG_DEFAULT, SMALL_DEFAULT, ORANGE, GREEN, PURPLE };

struct Style_info
{
	int width;
	int height;
	ID2D1Bitmap* Bitmap;
	int speed;
	Style_info(int width, int height, int speed, ID2D1Bitmap* pBitmap) :
		width(width), height(height), speed(speed), Bitmap(pBitmap) {};
};


extern map<TankStyle, Style_info*> Tank_Style_info;
extern map<BulletStyle, Style_info*> Bullet_Style_info;

bool InitStyle();