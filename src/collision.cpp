#include "collision.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "header.h"

struct Vec
{
	double x;
	double y;
	Vec(double x, double y) :x(x), y(y) {}
};
double dot(Vec Vec1, Vec  Vec2)
{
	return (fabs(Vec1.x * Vec2.x + Vec1.y * Vec2.y));
}


/*
* 判断两AABB包围盒的碰撞
* 输入：两个AABB包围盒的中心坐标x、y、宽、高、
* 输出：碰撞结果（true为发生碰撞，false为未发生碰撞）
*/
bool collision(double x1, double y1, int width1, int height1, double x2, double y2, int width2, int height2)
{
	//检测Y轴碰撞,碰撞则返回true;
	bool collisionY = y1 - height1 / 2 < y2 + height2 / 2 == y1 + height1 / 2 > y2 - height2 / 2;
	//检测X轴碰撞,碰撞则返回true
	bool collisionX = x1 - width1 / 2 < x2 + width2 / 2 == x1 + width1 / 2 > x2 - width2 / 2;

	if (collisionX && collisionY)
	{
		return true;
	}
	return false;
}


/*
* 判断两OOB包围盒的碰撞
* 输入：两个OOB包围盒的中心坐标x、y、宽、高、角度
* 输出：碰撞结果（true为发生碰撞，false为未发生碰撞）
*/
bool collision_obb(
	double x1, double y1,
	int width1, int height1, double rotate1,
	double x2, double y2,
	int width2, int height2, double rotate2)
{
	if ((rotate1 == 90 || rotate1 == 270) && rotate2 == 90)
		return collision(x1, y1, width1, height1, x2, y2, width2, height2);

	double sin_width1 = sin(rotate1 * M_PI / 180) * width1 / 2;
	double cos_width1 = cos(rotate1 * M_PI / 180) * width1 / 2;
	double cos_height1 = cos(rotate1 * M_PI / 180) * height1 / 2;
	double sin_height1 = sin(rotate1 * M_PI / 180) * height1 / 2;


	double sin_width2 = sin(rotate2 * M_PI / 180) * width2 / 2;
	double cos_width2 = cos(rotate2 * M_PI / 180) * width2 / 2;
	double cos_height2 = cos(rotate2 * M_PI / 180) * height2 / 2;
	double sin_height2 = sin(rotate2 * M_PI / 180) * height2 / 2;

	Vec axis[4] = {
		{ cos_height1,sin_height1 },	//矩形1沿着height方向的向量，模为height1/2,
		{ sin_width1, -cos_width1 },		//矩形1沿着width方向的向量，模为width1/2，
		{ cos_height2, sin_height2 },	//矩形2沿着height方向的向量，模为height2/2,
		{ sin_width2, -cos_width2 }		//矩形2沿着width方向的向量，模为width2/2，
	};

	Vec centre = { x1 - x2,y1 - y2 };	//两矩形中心连线方向的向量

	for (int i = 0; i < 4; i++)
	{
		if (dot(centre, axis[i]) >= dot(axis[0], axis[i]) + dot(axis[1], axis[i]) + dot(axis[2], axis[i]) + dot(axis[3], axis[i]))
			return false;
	}
	return true;
}
