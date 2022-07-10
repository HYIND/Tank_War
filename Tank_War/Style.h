#pragma once
#include "D2D.h"
using namespace std;

enum class TankStyle:uint8_t { DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };
enum class BulletStyle:uint8_t { DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };

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

void Init_Style();