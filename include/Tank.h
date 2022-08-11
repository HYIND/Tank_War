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
    double locationX;
    double locationY;
    double rotate;
    BulletStyle bullet_style = BulletStyle::DEFAULT;

    bullet *next = NULL;

    bullet(){};
    bullet(double locationX, double locationY, double rotate, BulletStyle bulletstyle = BulletStyle::DEFAULT)
        : locationX(locationX), locationY(locationY), rotate(rotate), bullet_style(bulletstyle) {}
};

class Tank
{
public:
    double locationX = 0;
    double locationY = 0;
    int width = 0;
    int height = 0;
    double rotate = 0;
    TankStyle tank_style = TankStyle::DEFAULT;
    int health = 100;

    bullet *bullet_head = NULL;

    bool isalive = true;
    // void *bullet_head=NULL;
};