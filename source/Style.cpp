#include "Style.h"

map<TankStyle, Style_info *> Tank_Style_info;
map<BulletStyle, Style_info *> Bullet_Style_info;

void Init_Style()
{
    /* Tank风格资源 */
    {
        Tank_Style_info[TankStyle::DEFAULT] = new Style_info(60, 60);
    }

    /* bullet风格资源 */
    {
        Bullet_Style_info[BulletStyle::DEFAULT] = new Style_info(30, 30);
    }
}