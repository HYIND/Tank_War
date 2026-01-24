#pragma once

#include "StateManager/UserManager.h"
#include "StateManager/RoomManager.h"
#include "StateManager/LobbyManager.h"

class GameStateManager
{
private:
    UserManager user_manager_;
    RoomManager room_manager_;
    LobbyManager lobby_manager_;

public:
    // 用户管理接口
    bool user_login(const std::string &name, const std::string &token)
    {
        if (user_manager_.add_user(name, token))
        {
            auto user = user_manager_.get_user(token);
            return lobby_manager_.player_join_lobby(user);
        }
        return false;
    }

    bool user_logout(const std::string &token)
    {
        auto user = user_manager_.get_user(token);
        if (user)
        {
            if (auto room = user->room.lock())
            {
                room_manager_.player_leave_room(room->room_id, token);
            }

            // 退出大厅
            lobby_manager_.player_leave_lobby(token);

            // 从用户管理器移除
            return user_manager_.remove_user(token);
        }
        return false;
    }

    // 房间管理接口
    bool create_room(const std::string &room_id, const std::string &room_name,
                     int max_players)
    {
        return room_manager_.add_room(room_id, room_name, max_players);
    }

    bool join_room(const std::string &room_id, const std::string &token)
    {
        auto user = user_manager_.get_user(token);
        if (!user)
            return false;

        // 先退出大厅
        lobby_manager_.player_leave_lobby(token);

        // 加入房间
        return room_manager_.player_enter_room(room_id, user);
    }

    bool leave_room(const std::string &room_id, const std::string &token)
    {
        if (room_manager_.player_leave_room(room_id, token))
        {
            auto user = user_manager_.get_user(token);
            if (user)
            {
                // 重新加入大厅
                return lobby_manager_.player_join_lobby(user);
            }
        }
        return false;
    }

    // 获取管理器引用（用于直接操作）
    UserManager &get_user_manager() { return user_manager_; }
    RoomManager &get_room_manager() { return room_manager_; }
    LobbyManager &get_lobby_manager() { return lobby_manager_; }

    // 统计数据
    struct GameStats
    {
        uint32_t total_users;
        uint32_t online_users;
        uint32_t total_rooms;
    };

    GameStats get_stats() const
    {
        GameStats stats;
        stats.total_users = user_manager_.usercount();
        stats.online_users = lobby_manager_.onlineusercount();
        stats.total_rooms = room_manager_.roomcount();
        return stats;
    }
};