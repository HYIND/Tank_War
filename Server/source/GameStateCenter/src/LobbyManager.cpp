
#include "StateManager/LobbyManager.h"

using namespace GameStateDef;

LobbyManager::LobbyManager()
{
    lobby_.online_count = 0;
    lobby_.max_online = 10000; // 默认最大在线人数
}

// 玩家加入大厅
bool LobbyManager::player_join_lobby(UserPtr user)
{
    if (!user)
        return false;

    // 检查是否已在大厅
    for (const auto &weak_user : lobby_.users)
    {
        if (auto existing = weak_user.lock())
        {
            if (existing->token == user->token && existing->name == user->name)
            {
                return false; // 已在列表中
            }
        }
    }

    lobby_.users.push_back(user);
    lobby_.online_count++;
    user->status == UserStatus::IN_LOBBY;
    return true;
}

// 玩家退出大厅
bool LobbyManager::player_leave_lobby(const std::string &user_id)
{
    for (auto it = lobby_.users.begin(); it != lobby_.users.end(); ++it)
    {
        if (auto user = it->lock())
        {
            if (user->name == user_id)
            { // 用name暂代user_id
                lobby_.users.erase(it);
                lobby_.online_count--;
                return true;
            }
        }
    }
    return false; // 用户不在大厅中
}

// 发布大厅公告（简化版本）
void LobbyManager::publish_announcement(const std::string &content)
{
    // 这里可以扩展为存储公告历史
    // 现在只是简单的广播功能占位
}

// 获取大厅信息
const Lobby &LobbyManager::get_lobby() const
{
    return lobby_;
}

// 获取大厅内的有效用户列表
std::vector<UserPtr> LobbyManager::get_valid_users()
{
    std::vector<UserPtr> valid_users;
    for (const auto &weak_user : lobby_.users)
    {
        if (auto user = weak_user.lock())
        {
            valid_users.push_back(user);
        }
    }
    return valid_users;
}

uint32_t LobbyManager::onlineusercount() const
{
    return lobby_.online_count;
}
