#include "ApplicationLayerCommunication/JsonProtocolSession.h"

JsonProtocolSession::JsonProtocolSession()
    : _owner(nullptr)
{
}

JsonProtocolSession::JsonProtocolSession(const std::string &sessionId, JsonProtocolServer *owner_server)
    : _sessionId(sessionId), _owner(owner_server)
{
}

bool JsonProtocolSession::AsyncSendJson(const json &js)
{
    if (!_owner)
        return false;

    return _owner->AsyncSend(*this, js);
}

bool JsonProtocolSession::AwaitSendJson(const json &req, json &resp)
{
    if (!_owner)
        return false;

    return _owner->AwaitSend(*this, req, resp);
}

bool JsonProtocolSession::Release()
{
    return _owner->ReleaseSession(*this);
}

std::string JsonProtocolSession::getsessionId() const
{
    return _sessionId;
}

JsonProtocolServer *JsonProtocolSession::getOwner() const
{
    return _owner;
}

bool JsonProtocolSession::operator==(const JsonProtocolSession &other) const
{
    return _sessionId == other._sessionId && _owner == other._owner;
}

bool JsonProtocolSession::operator!=(const JsonProtocolSession &other) const
{
    return !(*this == other);
}

bool JsonProtocolSession::isValid() const
{
    return !_sessionId.empty() && _owner != nullptr;
}

JsonProtocolSession::operator bool() const
{
    return isValid();
}