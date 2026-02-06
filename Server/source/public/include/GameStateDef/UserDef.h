#pragma once

#include "RoomDef.h"
#include <memory>
#include <string>
#include <unordered_map>
#include "ServiceRegistryData.h"

// 前向声明

namespace GameStateDef
{
    struct UserConnectedServiceInfo
    {
        std::string service_id;                           // 服务唯一ID
        ServiceRegistryDataDef::ServiceType service_type; // 服务类型
    };

    struct Room;

    enum class UserStatus
    {
        None = 0,
        IN_LOBBY, // 大厅中
        IN_ROOM,  // 房间中
        IN_GAME   // 游戏中
    };

    struct User
    {
        std::string token; // 用户令牌
        std::string name;  // 用户名

        UserStatus status;        // 用户状态
        std::weak_ptr<Room> room; // 当前所在房间的弱引用
        std::unordered_map<std::string, UserConnectedServiceInfo> connectedservices;
    };

    using UserPtr = std::shared_ptr<User>;
    using UserWeakPtr = std::weak_ptr<User>;

} // namespace GameStateDef