#include "GameStateManager.h"

bool GameStateManager::user_login(const std::string &name, const std::string &token)
{
    if (user_manager_.add_user(name, token))
    {
        auto user = user_manager_.get_user(token);
        return lobby_manager_.player_join_lobby(user);
    }
    return false;
}

// 房间管理接口
bool GameStateManager::create_room(const std::string &room_id, const std::string &room_name,
                                   int max_players)
{
    return room_manager_.add_room(room_id, room_name, max_players);
}

bool GameStateManager::join_room(const std::string &room_id, const std::string &token)
{
    auto user = user_manager_.get_user(token);
    if (!user)
        return false;

    if (user->status != UserStatus::IN_LOBBY)
        return false;

    // 加入房间
    return room_manager_.player_enter_room(room_id, user);
}

bool GameStateManager::leave_room(const std::string &room_id, const std::string &token)
{
    auto user = user_manager_.get_user(token);
    if (!user)
        return false;

    if (user->status != UserStatus::IN_ROOM)
        return false;

    return room_manager_.player_leave_room(room_id, user);
}

bool GameStateManager::change_ready_status(const std::string &room_id, const std::string &token, bool isready)
{
    auto user = user_manager_.get_user(token);
    if (!user)
        return false;

    if (user->status != UserStatus::IN_ROOM)
        return false;

    return room_manager_.change_ready_status(room_id, user, isready);
}

// 获取管理器引用（用于直接操作）
UserManager &GameStateManager::get_user_manager() { return user_manager_; }
RoomManager &GameStateManager::get_room_manager() { return room_manager_; }
LobbyManager &GameStateManager::get_lobby_manager() { return lobby_manager_; }

GameStateManager::GameStats GameStateManager::get_stats() const
{
    GameStats stats;
    stats.total_users = user_manager_.usercount();
    stats.online_users = lobby_manager_.onlineusercount();
    stats.total_rooms = room_manager_.roomcount();
    return stats;
}

bool GameStateManager::GetUserEndPoint(const std::string &token, std::vector<GameStateDef::UserConnectedServiceInfo> &userserviceinfos)
{
    UserPtr user = user_manager_.get_user(token);
    if (!user)
        return false;

    for (auto &pair : user->connectedservices)
        userserviceinfos.emplace_back(pair.second);

    return true;
}
