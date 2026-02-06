#pragma once

#include "GameStateDef/UserDef.h"
#include <unordered_map>
#include <memory>

using namespace GameStateDef;

class UserManager
{
private:
    std::unordered_map<std::string, UserPtr> users_; // token->user

public:
    // 添加新用户（用户登录）
    bool add_user(const std::string &name, const std::string &token);
    // 移除用户（用户退出）
    bool remove_user(const std::string &token);
    // 用户退出某一服务
    bool remove_user_from_service(const std::string &token, const std::string &serviceid);
    // 获取用户
    UserPtr get_user(const std::string &token);
    // 更新用户状态
    bool update_user_status(const std::string &token, UserStatus new_status);
    // 获取所有用户
    std::vector<UserPtr> get_all_users();
    // 获取在大厅的用户
    std::vector<UserPtr> get_all_lobby_users();
    // 检查用户是否存在
    bool has_user(const std::string &token);
    uint32_t usercount() const;
};