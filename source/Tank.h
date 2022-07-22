#pragma once
#include "Style.h"

enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
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

    bool isalive = true;
    bool isregister = false;
    // void *bullet_head=NULL;
};