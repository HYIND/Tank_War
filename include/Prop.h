#include "header.h"

using namespace std;

struct Prop     //道具类，记录道具类型，道具坐标以及过期时间
{
    int type;
    int id;

    int locationX;
    int locationY;
    time_t overtime;

    Prop(int type, int id, int x, int y)
        : type(type), id(id), locationX(x), locationY(y)
    {
        time(&overtime);
        overtime += 30;
    }
};

class PropManager
{
private:
    vector<Prop*> Prop_Vec;
public:
    PropManager();
    ~PropManager();

    // Create_Prop  
};
