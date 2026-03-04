#include "ApplicationLayerCommunication/JsonProtocolServer.h"

JsonProtocolSession::JsonProtocolSession()
    : _owner(nullptr)
{
}

JsonProtocolSession::JsonProtocolSession(const JsonProtocolSessionID& sessionId, JsonProtocolServer* owner_server)
    : _sessionId(sessionId), _owner(owner_server)
{
}

bool JsonProtocolSession::AsyncSendJson(const json& js)
{
    if (!_owner)
        return false;

    return _owner->AsyncSend(*this, js);
}

Task<bool> JsonProtocolSession::AwaitSendJson(const json& req, json& resp)
{
    if (!_owner)
        co_return false;

    co_return co_await _owner->AwaitSend(*this, req, resp);
}

bool JsonProtocolSession::Release()
{
    return _owner->ReleaseSession(*this);
}

JsonProtocolSessionID JsonProtocolSession::getsessionId() const
{
    return _sessionId;
}

JsonProtocolServer* JsonProtocolSession::getOwner() const
{
    return _owner;
}

bool JsonProtocolSession::operator==(const JsonProtocolSession& other) const
{
    return _sessionId == other._sessionId && _owner == other._owner;
}

bool JsonProtocolSession::operator!=(const JsonProtocolSession& other) const
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

JsonProtocolServer::JsonProtocolServer()
{
    _sessionmanager.SetCallBackRecvMessage(std::bind(&JsonProtocolServer::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _sessionmanager.SetCallBackRequestMessage(std::bind(&JsonProtocolServer::RequestMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _sessionmanager.SetCallBackSessionEstablish(std::bind(&JsonProtocolServer::ConnectionEstablish, this, std::placeholders::_1));
    _sessionmanager.SetCallBackCloseConnect(std::bind(&JsonProtocolServer::CloseConnect, this, std::placeholders::_1));
}

JsonProtocolServer::~JsonProtocolServer()
{
}

bool JsonProtocolServer::Start(const std::string &IP, int Port)
{
    return _sessionmanager.Start(IP, Port);
}

bool JsonProtocolServer::SetCallBackRecvJsonMessage(const JsonProtocolSession &session, CallBackRecvJsonMessage &&callback)
{
    if (!IsValidSession(session))
        return false;

    std::shared_ptr<JsonProtocolServer::SessionHandle> handle;
    if (!_SessionIdToSessionHandle.FindByLeft(session.getsessionId(), handle))
        return false;

    handle->c_message = callback;
    return true;
}

bool JsonProtocolServer::SetCallBackRecvJsonRequest(const JsonProtocolSession &session, CallBackRecvJsonRequest &&callback)
{
    if (!IsValidSession(session))
        return false;

    std::shared_ptr<JsonProtocolServer::SessionHandle> handle;
    if (!_SessionIdToSessionHandle.FindByLeft(session.getsessionId(), handle))
        return false;

    handle->c_request = callback;
    return true;
}

bool JsonProtocolServer::SetCallBackCloseConnect(const JsonProtocolSession &session, CallBackCloseConnect &&callback)
{
    if (!IsValidSession(session))
        return false;

    std::shared_ptr<JsonProtocolServer::SessionHandle> handle;
    if (!_SessionIdToSessionHandle.FindByLeft(session.getsessionId(), handle))
        return false;

    handle->c_closeconnect = callback;
    return true;
}

void JsonProtocolServer::SetCallbackSessionEstablish(CallBackSessionEstablish &&callback)
{
    _CallBackSessionEstablish = callback;
}

bool JsonProtocolServer::AsyncSend(const JsonProtocolSession &session, const json &js)
{
    if (!IsValidSession(session))
        return false;

    ConID conId;
    if (!_ConIdToSessionId.FindByRight(session.getsessionId(), conId) || conId.empty())
        return false;

    return _sessionmanager.AsyncSend(conId, Buffer(js.dump()));
}

Task<bool> JsonProtocolServer::AwaitSend(const JsonProtocolSession &session, const json &req, json &rsp)
{
    if (!IsValidSession(session))
        co_return false;

    ConID conId;
    if (!_ConIdToSessionId.FindByRight(session.getsessionId(), conId) || conId.empty())
        co_return false;

    Buffer resp;
    if (!co_await _sessionmanager.AwaitSend(conId, Buffer(req.dump()), resp))
        co_return false;

    co_return ParseJson(resp, rsp);
}

bool JsonProtocolServer::ReleaseSession(const JsonProtocolSessionID &sessionId)
{
    ConID conId;
    {
        auto guard1 = _SessionIdToSessionHandle.MakeLockGuard();
        auto guard2 = _ConIdToSessionId.MakeLockGuard();

        std::shared_ptr<JsonProtocolServer::SessionHandle> temp;
        if (!_SessionIdToSessionHandle.FindByLeft(sessionId, temp) || !temp)
            return false;

        _SessionIdToSessionHandle.EraseByLeft(sessionId);

        if (!_ConIdToSessionId.FindByRight(sessionId, conId) || conId.empty())
            return false;
    }

    return _sessionmanager.ReleaseSession(conId);
}

bool JsonProtocolServer::ReleaseSession(const JsonProtocolSession &session)
{
    if (!IsValidSession(session))
        return false;

    return ReleaseSession(session.getsessionId());
}

Task<void> JsonProtocolServer::RecvMessage(ConID conId, Buffer *recv)
{
    if (_waitConId.Exist(conId))
    {
        json js_src;
        if (!ParseJson(*recv, js_src))
            co_return;
        json js_dest;

        if (!js_src.contains("command") || !js_src["command"].is_number_integer())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "命令号缺失！";
            _sessionmanager.AsyncSend(conId, Buffer(js_dest.dump()));
            co_return;
        }
        int command = js_src.at("command");
        if (command == 999888)
        {
            JsonProtocolSessionID sessionId;
            {
                auto guard1 = _SessionIdToSessionHandle.MakeLockGuard();
                auto guard2 = _ConIdToSessionId.MakeLockGuard();
                bool needcreate = false;
                if (js_src.contains("sessionid") && js_src["sessionid"].is_string())
                {
                    sessionId = js_src.value("sessionid", "");
                    std::shared_ptr<SessionHandle> handle;
                    if (_SessionIdToSessionHandle.FindByLeft(sessionId, handle) && handle)
                    {
                        handle->connectionId = conId;
                        handle->lastActiveTimeSecond = Tool::GetTimestampSecond();

                        _ConIdToSessionId.InsertOrUpdate(conId, sessionId);
                        _waitConId.Erase(conId);
                    }
                    else
                        needcreate = true;
                }
                else
                    needcreate = true;

                if (needcreate)
                {
                    sessionId = Tool::GenerateSimpleUuid();
                    auto handle = std::make_shared<SessionHandle>();
                    handle->connectionId = conId;
                    handle->sessionEstablishTimeSecond = Tool::GetTimestampSecond();
                    handle->lastActiveTimeSecond = Tool::GetTimestampSecond();
                    _SessionIdToSessionHandle.InsertOrUpdate(sessionId, handle);
                    _ConIdToSessionId.InsertOrUpdate(conId, sessionId);
                    _waitConId.Erase(conId);
                }
                js_dest["result"] = 1;
                js_dest["sessionid"] = sessionId;
                _sessionmanager.AsyncSend(conId, Buffer(js_dest.dump()));
            }
            JsonProtocolSession newsession(sessionId, this);
            if (newsession)
            {
                auto callback = _CallBackSessionEstablish;
                if (callback)
                    co_await callback(newsession);
            }
            co_return;
        }
        else
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "无效命令号！";
            _sessionmanager.AsyncSend(conId, Buffer(js_dest.dump()));
            co_return;
        }
    }
    else
    {
        json js_src;
        if (!ParseJson(*recv, js_src))
            co_return;

        JsonProtocolSessionID sessionId;
        if (!_ConIdToSessionId.FindByLeft(conId, sessionId) || sessionId.empty())
            co_return;

        std::shared_ptr<SessionHandle> handle;
        if (!_SessionIdToSessionHandle.FindByLeft(sessionId, handle) || !handle)
            co_return;

        handle->lastActiveTimeSecond = Tool::GetTimestampSecond();
        auto callback = handle->c_message;
        if (callback)
            co_await callback(JsonProtocolSession(sessionId, this), js_src);
    }
}

Task<void> JsonProtocolServer::RequestMessage(ConID conId, Buffer *recv, Buffer *resp)
{
    json js_src;
    if (!ParseJson(*recv, js_src))
        co_return;

    if (_waitConId.Exist(conId))
    {
        json js_dest;

        if (!js_src.contains("command") || !js_src["command"].is_number_integer())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "命令号缺失！";
            resp->QuoteFromBuf(Buffer(js_dest.dump()));
            co_return;
        }
        int command = js_src.at("command");
        if (command == 999888)
        {
            JsonProtocolSessionID sessionId;
            {
                auto guard1 = _SessionIdToSessionHandle.MakeLockGuard();
                auto guard2 = _ConIdToSessionId.MakeLockGuard();
                bool needcreate = false;
                if (js_src.contains("sessionid") && js_src["sessionid"].is_string())
                {
                    sessionId = js_src.value("sessionid", "");
                    std::shared_ptr<SessionHandle> handle;
                    if (_SessionIdToSessionHandle.FindByLeft(sessionId, handle) && handle)
                    {
                        handle->connectionId = conId;
                        handle->lastActiveTimeSecond = Tool::GetTimestampSecond();

                        _ConIdToSessionId.InsertOrUpdate(conId, sessionId);
                        _waitConId.Erase(conId);
                    }
                    else
                        needcreate = true;
                }
                else
                    needcreate = true;

                if (needcreate)
                {
                    sessionId = Tool::GenerateSimpleUuid();
                    auto handle = std::make_shared<SessionHandle>();
                    handle->connectionId = conId;
                    handle->sessionEstablishTimeSecond = Tool::GetTimestampSecond();
                    handle->lastActiveTimeSecond = Tool::GetTimestampSecond();
                    _SessionIdToSessionHandle.InsertOrUpdate(sessionId, handle);
                    _ConIdToSessionId.InsertOrUpdate(conId, sessionId);
                    _waitConId.Erase(conId);
                }
                js_dest["result"] = 1;
                js_dest["sessionid"] = sessionId;
                resp->QuoteFromBuf(Buffer(js_dest.dump()));
            }

            JsonProtocolSession newsession(sessionId, this);
            if (newsession)
            {
                auto callback = _CallBackSessionEstablish;
                if (callback)
                    co_await callback(newsession);
            }

            co_return;
        }
        else
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "无效命令号！";
            resp->QuoteFromBuf(Buffer(js_dest.dump()));
            co_return;
        }
    }
    else
    {

        JsonProtocolSessionID sessionId;
        if (!_ConIdToSessionId.FindByLeft(conId, sessionId) || sessionId.empty())
            co_return;

        std::shared_ptr<SessionHandle> handle;
        if (!_SessionIdToSessionHandle.FindByLeft(sessionId, handle)|| !handle)
            co_return;

        handle->lastActiveTimeSecond = Tool::GetTimestampSecond();
        auto callback = handle->c_request;
        if (callback)
        {
            json js_resp;
            co_await callback(JsonProtocolSession(sessionId, this), js_src, js_resp);
            resp->QuoteFromBuf(Buffer(js_resp.dump()));
        }
    }
}

Task<void> JsonProtocolServer::CloseConnect(ConID conId)
{
    if (_waitConId.Exist(conId))
    {
        _waitConId.Erase(conId);
    }
    else
    {

        JsonProtocolSessionID sessionId;
        if (!_ConIdToSessionId.FindByLeft(conId, sessionId) || sessionId.empty())
            co_return;

        std::shared_ptr<SessionHandle> handle;
        if (_SessionIdToSessionHandle.FindByLeft(sessionId, handle) && handle)
        {
            auto callback = handle->c_closeconnect;
            if (callback)
                co_await callback(JsonProtocolSession(sessionId, this));
        }
    }
}

Task<void> JsonProtocolServer::ConnectionEstablish(ConID conId)
{
    _waitConId.Insert(conId);
    co_return;
}

bool JsonProtocolServer::ParseJson(Buffer &buf, json &js)
{
    std::string js_str(buf.Byte(), buf.Length());
    try
    {
        js = json::parse(js_str);
        return true;
    }
    catch (...)
    {
        std::cout << std::format("JsonProtocolServer ParseJson error : {}\n", js_str);
        return false;
    }
}

bool JsonProtocolServer::IsValidSession(const JsonProtocolSession &session)
{
    return !session.getsessionId().empty() && session.getOwner() == this;
}
