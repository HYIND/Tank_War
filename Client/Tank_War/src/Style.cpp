#include "Style.h"

map<TankStyle, Style_info*> Tank_Style_info;
map<BulletStyle, Style_info*> Bullet_Style_info;;

bool Init_Style()
{
	/* Tank风格资源 */
	{
		Tank_Style_info[TankStyle::DEFAULT] = new Style_info(50, 50, 5, ResFactory->GetBitMapRes(ResName::defTank));
	}

	/* bullet风格资源 */
	{
		Bullet_Style_info[BulletStyle::DEFAULT] = new Style_info(25, 25, 7, ResFactory->GetBitMapRes(ResName::defBullet));
		Bullet_Style_info[BulletStyle::ORANGE] = new Style_info(50, 95, 7, ResFactory->GetBitMapRes(ResName::orangeBullet));
		Bullet_Style_info[BulletStyle::GREEN] = new Style_info(50, 95, 7, ResFactory->GetBitMapRes(ResName::greenBullet));
		Bullet_Style_info[BulletStyle::PURPLE] = new Style_info(50, 95, 7, ResFactory->GetBitMapRes(ResName::purpleBullet));
	}
	return true;
}