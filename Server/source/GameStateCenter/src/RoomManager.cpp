#include "StateManager/RoomManager.h"

// 添加新房间
bool RoomManager::add_room(const std::string &room_id, const std::string &room_name, int max_players)
{
    if (rooms_.find(room_id) != rooms_.end())
    {
        return false; // 房间已存在
    }

    auto room = std::make_shared<Room>();
    room->room_id = room_id;
    room->room_name = room_name;
    room->max_players = max_players;
    room->player_count = 0;
    room->status = RoomStatus::READY; // 新房间默认准备中

    rooms_[room_id] = room;
    return true;
}

// 玩家进入房间
bool RoomManager::player_enter_room(const std::string &room_id, UserPtr user)
{
    auto room = get_room(room_id);
    if (!room || !user)
        return false;

    // 检查房间是否已满
    if (room->player_count >= room->max_players)
    {
        return false;
    }

    for (auto it = room->members.begin(); it != room->members.end();)
    {
        auto &member = *it;
        auto roomuser = member.user.lock();
        if (!user)
        {
            it = room->members.erase(it);
            room->player_count--;
        }
        else if (user->token == roomuser->token)
        {
            return false;
        }
        it++;
    }

    // 添加用户到房间
    room->members.push_back(RoomMemeber{user, MemberStatus::READY});
    room->player_count++;

    // 更新用户状态和房间引用
    user->status = UserStatus::IN_ROOM;
    user->room = room;

    return true;
}

// 玩家退出房间
bool RoomManager::player_leave_room(const std::string &room_id, const std::string &token)
{
    auto room = get_room(room_id);
    if (!room)
        return false;

    // 查找并移除用户
    bool result = false;
    for (auto it = room->members.begin(); it != room->members.end();)
    {
        auto &member = *it;
        auto user = member.user.lock();
        bool needremove = (!user || user->token == token); // 顺便清理引用失效用户
        if (needremove)
        {
            it = room->members.erase(it);
            room->player_count--;

            if (user)
            {
                result = true;
                user->status = UserStatus::IN_LOBBY;
                user->room.reset();
            }
            continue;
        }
        ++it;
    }

    return result; // 用户不在房间中
}

// 获取房间
RoomPtr RoomManager::get_room(const std::string &room_id)
{
    auto it = rooms_.find(room_id);
    return it != rooms_.end() ? it->second : nullptr;
}

// 获取所有房间
std::vector<RoomPtr> RoomManager::get_all_rooms()
{
    std::vector<RoomPtr> result;
    for (auto &[id, room] : rooms_)
    {
        result.push_back(room);
    }
    return result;
}

// 删除房间
bool RoomManager::remove_room(const std::string &room_id)
{
    auto room = get_room(room_id);
    if (!room)
        return false;

    // 清理房间内所有用户的房间引用
    for (auto &member : room->members)
    {
        if (auto user = member.user.lock())
        {
            user->room.reset();
            user->status = UserStatus::IN_LOBBY;
        }
    }

    rooms_.erase(room_id);
    return true;
}

uint32_t RoomManager::roomcount() const
{
    return rooms_.size();
}
