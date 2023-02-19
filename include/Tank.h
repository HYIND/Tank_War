#pragma once

#include "Style.h"
#include "Bullet.h"
#include "Object.h"

class bullet;

class Tank : public RectObject
{
public:
    TankStyle tank_style = TankStyle::DEFAULT;
    int health = 100;

    Bullet *bullet_head = NULL;

    bool isalive = true;
};