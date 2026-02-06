#pragma once

#include "GameStateDef/LobbyDef.h"
#include "GameStateDef/UserDef.h"
#include <memory>

using namespace GameStateDef;

class LobbyManager
{
private:
    Lobby lobby_; // 单个大厅

public:
    LobbyManager();

    // 玩家加入大厅
    bool player_join_lobby(UserPtr user);
    // 玩家退出大厅
    bool player_leave_lobby(const std::string &user_id);
    // 发布大厅公告（简化版本）
    void publish_announcement(const std::string &content);
    // 获取大厅信息
    const Lobby &get_lobby() const;
    // 获取大厅内的有效用户列表
    std::vector<UserPtr> get_valid_users();
    uint32_t onlineusercount() const;
};
