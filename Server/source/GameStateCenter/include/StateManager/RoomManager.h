#pragma once

#include "GameStateDef/RoomDef.h"
#include <unordered_map>
#include <memory>

using namespace GameStateDef;

class RoomManager
{
private:
    std::unordered_map<std::string, RoomPtr> rooms_; // room_id -> Room

public:
    // 添加新房间
    bool add_room(const std::string &room_id, const std::string &room_name, int max_players);
    // 玩家进入房间
    bool player_enter_room(const std::string &room_id, UserPtr user);
    // 玩家退出房间
    bool player_leave_room(const std::string &room_id, UserPtr user);
    // 玩家准备/取消准备
    bool change_ready_status(const std::string &room_id, UserPtr user, bool isready);

    // 获取房间
    RoomPtr get_room(const std::string &room_id);
    // 获取所有房间
    std::vector<RoomPtr> get_all_rooms();
    // 删除房间
    bool remove_room(const std::string &room_id);
    uint32_t roomcount() const;
};
