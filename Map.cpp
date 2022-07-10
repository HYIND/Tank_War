#include "Map.h"

map<int, Map> Map_list;

void Init_Map()
{
    Map Default_Map(0, 2);
    Default_Map.Init_Location.emplace_back(1, 50, 300, UP, TankStyle::DEFAULT);
    Default_Map.Init_Location.emplace_back(2, 500, 300, UP, TankStyle::DEFAULT);
    Map_list[Default_Map.map_id] = Default_Map;
}
