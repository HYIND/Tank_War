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

    if (room->status == RoomStatus::IN_GAME)
        return false;

    auto guard = room->members.MakeLockGuard();

    if (room->status == RoomStatus::IN_GAME)
        return false;

    // 检查房间是否已满
    if (room->player_count >= room->max_players)
    {
        return false;
    }

    if (room->members.Exist(user->token))
        return false;

    // 添加用户到房间
    auto roommember = std::make_shared<RoomMemeber>();
    roommember->userweak = user;
    roommember->name = user->name;
    roommember->status = MemberStatus::NO_READY;

    room->members.Insert(user->token, roommember);
    room->player_count = room->members.Size();

    // 更新用户状态和房间引用
    user->status = UserStatus::IN_ROOM;
    user->room = room;

    if (room->player_count == 1)
    {
        room->room_host_token = user->token;
        roommember->status = MemberStatus::READY;
    }

    return true;
}

// 玩家退出房间
bool RoomManager::player_leave_room(const std::string &room_id, UserPtr user)
{
    auto room = get_room(room_id);
    if (!room)
        return false;

    if (room->status == RoomStatus::IN_GAME)
        return false;

    auto guard = room->members.MakeLockGuard();

    if (room->status == RoomStatus::IN_GAME)
        return false;

    if (!room->members.Exist(user->token))
        return false;

    std::shared_ptr<RoomMemeber> member;
    if (room->members.Find(user->token, member))
    {
        auto user = member->userweak.lock();
        if (user)
        {
            user->status = UserStatus::IN_LOBBY;
            user->room.reset();
        }
        room->members.Erase(user->token);
    }
    room->player_count = room->members.Size();

    if (room->player_count <= 0)
        remove_room(room_id);
    else
    {
        // 更换房主
        if (user->token == room->room_host_token && room->player_count > 0)
        {
            room->members.EnsureCall(
                [&](std::map<std::string, std::shared_ptr<GameStateDef::RoomMemeber>> &map) -> void
                {
                    auto it = map.begin();
                    if (it != map.end())
                    {
                        room->room_host_token = it->first;
                        it->second->status = MemberStatus::READY;
                    }
                });
        }
    }

    return true;
}

bool RoomManager::change_ready_status(const std::string &room_id, UserPtr user, bool isready)
{
    auto room = get_room(room_id);
    if (!room)
        return false;

    if (room->status == RoomStatus::IN_GAME)
        return false;

    if(room->room_host_token == room_id)
        return false;

    auto guard = room->members.MakeLockGuard();

    if (room->status == RoomStatus::IN_GAME)
        return false;

    if (!room->members.Exist(user->token))
        return false;

    std::shared_ptr<RoomMemeber> member;
    if (room->members.Find(user->token, member))
    {
        if (!member)
            return false;
        member->status = isready ? MemberStatus::READY : MemberStatus::NO_READY;
    }
    return true;
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

    auto guard = room->members.MakeLockGuard();
    // 清理房间内所有用户的房间引用
    room->members.EnsureCall(
        [](std::map<std::string, std::shared_ptr<RoomMemeber>> &map) -> void
        {
            for (auto &pair : map)
            {
                auto &member = pair.second;
                if (auto user = member->userweak.lock())
                {
                    user->status = UserStatus::IN_LOBBY;
                    user->room.reset();
                }
            }
        });

    rooms_.erase(room_id);
    return true;
}

uint32_t RoomManager::roomcount() const
{
    return rooms_.size();
}
