#pragma once
#include "header.h"

using namespace std;

enum class TankStyle:uint8_t{ DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };
enum class BulletStyle:uint8_t{ DEFAULT, BIG_DEFAULT, SMALL_DEFAULT };

struct Style_info
{
	int width;
	int height;
	Style_info(int width, int height) :
		width(width), height(height){};
};


extern map<TankStyle, Style_info*> Tank_Style_info;
extern map<BulletStyle, Style_info*> Bullet_Style_info;

void Init_Style();