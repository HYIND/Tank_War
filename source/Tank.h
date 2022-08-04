#pragma once
#include "Style.h"

enum Direction : int
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class bullet
{
public:
    int locationX;
    int locationY;
    int direction;
    BulletStyle bullet_style = BulletStyle::DEFAULT;

    bullet *next = NULL;

    bullet(){};
    bullet(int locationX, int locationY, int direction, BulletStyle bulletstyle = BulletStyle::DEFAULT)
        : locationX(locationX), locationY(locationY), direction(direction), bullet_style(bulletstyle) {}
};

class Tank
{
public:
    int locationX = 0;
    int locationY = 0;
    int width = 0;
    int height = 0;
    int direction = UP;
    TankStyle tank_style = TankStyle::DEFAULT;
    int health = 100;

    bullet *bullet_head = NULL;

    bool isalive = true;
    // void *bullet_head=NULL;
};