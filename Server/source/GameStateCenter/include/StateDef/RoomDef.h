#pragma once

#include "UserDef.h"
#include <vector>
#include <string>
#include <memory>

namespace GameStateDef
{
    struct User;
    using UserPtr = std::shared_ptr<User>;
    using UserWeakPtr = std::weak_ptr<User>;

    enum class RoomStatus
    {
        IN_GAME, // 游戏中
        READY    // 准备中
    };

    enum class MemberStatus
    {
        IN_GAME, // 游戏中
        READY    // 准备中
    };

    struct RoomMemeber
    {
        UserWeakPtr user; // 用户弱引用
        MemberStatus status;
    };

    struct Room
    {
        std::string room_id;   // 房间ID
        std::string room_name; // 房间名称

        int player_count; // 房间人数
        int max_players;  // 房间最大人数

        RoomStatus status;

        std::vector<RoomMemeber> members;
    };

    using RoomPtr = std::shared_ptr<Room>;
    using RoomWeakPtr = std::weak_ptr<Room>;

} // namespace GameStateDef