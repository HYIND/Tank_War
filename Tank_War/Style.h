#pragma once
#include "D2D.h"
using namespace std;

enum class TankStyle { DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };
enum class BulletStyle { DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };

struct Style_info
{
	int width;
	int height;
	ID2D1Bitmap* Bitmap;
	Style_info(int width, int height, ID2D1Bitmap* pBitmap) :
		width(width), height(height), Bitmap(pBitmap) {};
};

extern map<TankStyle, Style_info*> Tank_Style_info;
extern map<BulletStyle, Style_info*> Bullet_Style_info;

void Init_Style();