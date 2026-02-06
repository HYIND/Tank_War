#pragma once

#include "UserDef.h"
#include <vector>
#include <string>

namespace GameStateDef
{
    struct Lobby
    {
        int online_count; // 当前在线人数
        int max_online;   // 最大在线人数

        // 当前在大厅的玩家（弱引用）
        std::vector<UserWeakPtr> users;
    };

} // namespace GameStateDef