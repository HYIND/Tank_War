#pragma once
#include "header.h"
#include "Tank.h"

#define AIDKIT_WIDTH 40
#define AIDKIT_HEIGHT 40

extern ID2D1Bitmap* aidkit_pBitmap;

enum class prop_type :int { DEFAULT, AIDKIT };

class Prop     //道具类，记录道具类型，道具坐标以及过期时间
{
	friend class Game;
protected:
	time_t overtime;

	int locationX;
	int locationY;
	int width;
	int height;

	ID2D1Bitmap* Bitmap = NULL;

public:
	int id;
	prop_type type = prop_type::DEFAULT;

	Prop(int x, int y, int width, int height, ID2D1Bitmap* Bitmap, int id, prop_type type)
		: type(type), id(id), locationX(x), locationY(y)
	{
		time(&overtime);
		overtime += 30;
	}
	virtual void get(Tank* tank);
	virtual void online_get(int id);
	virtual void Draw();
	virtual ~Prop() {};
};

class aid_kit :public Prop {
	aid_kit(int x, int y, int id)
		:Prop(x, y, AIDKIT_WIDTH, AIDKIT_HEIGHT, aidkit_pBitmap, id, prop_type::AIDKIT) {};
	using Prop::Draw;;
};

void Init_Prop_Resource();