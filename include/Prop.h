#pragma once

#include "header.h"
#include "Game_Component.h"

#define AIDKIT_WIDTH 40
#define AIDKIT_HEIGHT 40
using namespace std;

class Prop : public Game_Component // 道具类，记录道具类型，道具坐标以及过期时间
{
protected:
    time_t overtime;

public:
    Prop(double x, double y, int width, int height, int id, component_type type = component_type::PROP_DEFAULT)
        : Game_Component(x, y, width, height, 999, id, type)
    {
        time(&overtime);
        overtime += 30;
    }
};

class Aid_kit : public Prop
{
public:
    Aid_kit(double x, double y, int id)
        : Prop(x, y, AIDKIT_WIDTH, AIDKIT_HEIGHT, id, component_type::AIDKIT){};
};

class PropManager
{

private:
    vector<Prop *> Prop_Vec;

public:
    PropManager();
    ~PropManager();

    // Create_Prop
};
