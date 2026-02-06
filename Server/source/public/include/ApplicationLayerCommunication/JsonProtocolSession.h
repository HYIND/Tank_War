#pragma once

#include "stdafx.h"
#include "JsonProtocolServer.h"

class JsonProtocolServer;

// 由JsonProtocolServer进行连接管理的Json会话，由JsonProtocolServer管理生命周期
// 仅用来提供获取Session后主动发送数据或释放连接的功能
class JsonProtocolSession
{

public:
    JsonProtocolSession();
    JsonProtocolSession(const std::string &sessionId, JsonProtocolServer *owner_server);

    bool AsyncSendJson(const json &js);
    bool AwaitSendJson(const json &req, json &resp);

    bool Release();

    std::string getsessionId() const;
    JsonProtocolServer *getOwner() const;

    bool operator==(const JsonProtocolSession &other) const;
    bool operator!=(const JsonProtocolSession &other) const;

    bool isValid() const;

    explicit operator bool() const;

private:
    JsonProtocolServer *_owner;
    std::string _sessionId;
};

namespace std
{
    template <>
    struct hash<JsonProtocolSession>
    {
        size_t operator()(const JsonProtocolSession &session) const noexcept
        {
            return std::hash<std::string>{}(session.getsessionId());
        }
    };
}