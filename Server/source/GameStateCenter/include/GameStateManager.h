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
    bool user_login(const std::string &name, const std::string &token);
    bool user_logout(const std::string &token);

    // 房间管理接口
    bool create_room(const std::string &room_id, const std::string &room_name, int max_players);
    bool join_room(const std::string &room_id, const std::string &token);
    bool leave_room(const std::string &room_id, const std::string &token);
    bool change_ready_status(const std::string &room_id, const std::string &token, bool isready);

    bool leave_game(const std::string &room_id, const std::string &token);
    bool game_end(const std::string &gameid);

    // 获取管理器引用（用于直接操作）
    UserManager &get_user_manager();
    RoomManager &get_room_manager();
    LobbyManager &get_lobby_manager();

    // 统计数据
    struct GameStats
    {
        uint32_t total_users;
        uint32_t online_users;
        uint32_t total_rooms;
    };

    GameStats get_stats() const;

    bool GetUserEndPoint(const std::string &token, std::vector<GameStateDef::UserConnectedServiceInfo> &userserviceinfos);
};