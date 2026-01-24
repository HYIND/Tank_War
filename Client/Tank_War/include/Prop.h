#pragma once
#include "header.h"
#include "Tank.h"
#include "Game_Component.h"

#define AIDKIT_WIDTH 40
#define AIDKIT_HEIGHT 40


//道具类，记录道具类型，道具坐标以及过期时间
class Prop :public Game_Component
{
	friend class Game;
protected:
	time_t overtime;

public:

	Prop(double x, double y, int width, int height, int id, ID2D1Bitmap* Bitmap, component_type type = component_type::DEFAULT)
		: Game_Component(x, y, width, height, 999, id, Bitmap, type)
	{
		time(&overtime);
		overtime += 30;
	}
	virtual void get(Tank* tank);
	virtual void online_get(int id);
	virtual void Draw();
	virtual ~Prop() {};
};

class Aid_kit :public Prop
{
public:
	Aid_kit(double x, double y, int id)
		:Prop(x, y, AIDKIT_WIDTH, AIDKIT_HEIGHT, id, ResFactory->GetBitMapRes(ResName::aidKit), component_type::AIDKIT) {};
	using Prop::Draw;
};
