#include "collision.h"
//#include "box2d/box2d.h"

//b2Body* b2World::CreateBody(const b2BodyDef* def)
//b2Joint* b2World::CreateJoint(const b2JointDef* def)

bool collision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
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