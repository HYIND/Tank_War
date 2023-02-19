#pragma once

#include "header.h"
#include "Style.h"
#include "Object.h"
#include "Tank.h"

class Tank;

class Bullet : public RoundObject
{
public:
    BulletStyle bullet_style = BulletStyle::DEFAULT;

    Bullet *next = NULL;

    Bullet(){};
    Bullet(double x, double y, double rotate, BulletStyle bulletstyle = BulletStyle::DEFAULT)
        : RoundObject(x, y, 25, rotate), bullet_style(bulletstyle) {}
};