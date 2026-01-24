#include "StateManager/UserManager.h"
#include <unordered_map>
#include <memory>

// 添加新用户（用户登录）
bool UserManager::add_user(const std::string &name, const std::string &token)
{
    if (users_.find(token) != users_.end())
    {
        return false;
    }

    auto user = std::make_shared<User>();
    user->token = token;
    user->name = name;
    user->status = UserStatus::None;

    users_[token] = user;
    return true;
}

// 移除用户（用户退出）
bool UserManager::remove_user(const std::string &token)
{
    return users_.erase(token) > 0;
}

// 获取用户
UserPtr UserManager::get_user(const std::string &token)
{
    auto it = users_.find(token);
    return it != users_.end() ? it->second : nullptr;
}

// 更新用户状态
bool UserManager::update_user_status(const std::string &token, UserStatus new_status)
{
    auto user = get_user(token);
    if (!user)
        return false;

    user->status = new_status;
    return true;
}

// 获取所有用户
std::vector<UserPtr> UserManager::get_all_users()
{
    std::vector<UserPtr> result;
    for (auto &[id, user] : users_)
    {
        result.push_back(user);
    }
    return result;
}

// 检查用户是否存在
bool UserManager::has_user(const std::string &token)
{
    return users_.find(token) != users_.end();
}

uint32_t UserManager::usercount() const
{
    return users_.size();
}