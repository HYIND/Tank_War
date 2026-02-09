#include "ApplicationLayerCommunication/JsonProtocolServer.h"

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

    std::string conId;
    if (!_ConIdToSessionId.FindByRight(session.getsessionId(), conId) || conId.empty())
        return false;

    return _sessionmanager.AsyncSend(conId, Buffer(js.dump()));
}

bool JsonProtocolServer::AwaitSend(const JsonProtocolSession &session, const json &req, json &rsp)
{
    if (!IsValidSession(session))
        return false;

    std::string conId;
    if (!_ConIdToSessionId.FindByRight(session.getsessionId(), conId) || conId.empty())
        return false;

    Buffer resp;
    if (!_sessionmanager.AwaitSend(conId, Buffer(req.dump()), resp))
        return false;

    return ParseJson(resp, rsp);
}

bool JsonProtocolServer::ReleaseSession(const std::string &sessionId)
{
    std::string conId;
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

void JsonProtocolServer::RecvMessage(std::string conId, Buffer *recv)
{
    json js_src;
    if (!ParseJson(*recv, js_src))
        return;

    if (_waitConId.Exist(conId))
    {
        json js_dest;

        if (!js_src.contains("command") || !js_src["command"].is_number_integer())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "命令号缺失！";
            _sessionmanager.AsyncSend(conId, Buffer(js_dest.dump()));
            return;
        }
        int command = js_src.at("command");
        if (command == 999888)
        {
            auto guard1 = _SessionIdToSessionHandle.MakeLockGuard();
            auto guard2 = _ConIdToSessionId.MakeLockGuard();
            std::string sessionId;
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

            auto callback = _CallBackSessionEstablish;
            if (callback)
                std::invoke(callback, JsonProtocolSession(sessionId, this));

            return;
        }
        else
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "无效命令号！";
            _sessionmanager.AsyncSend(conId, Buffer(js_dest.dump()));
            return;
        }
    }
    else
    {
        std::string sessionId;
        if (!_ConIdToSessionId.FindByLeft(conId, sessionId) || sessionId.empty())
            return;

        std::shared_ptr<SessionHandle> handle;
        if (_SessionIdToSessionHandle.FindByLeft(sessionId, handle) && handle)
        {
            handle->lastActiveTimeSecond = Tool::GetTimestampSecond();
            auto callback = handle->c_message;
            if (callback)
                std::invoke(callback, JsonProtocolSession(sessionId, this), js_src);
        }
    }
}

void JsonProtocolServer::RequestMessage(std::string conId, Buffer *recv, Buffer *resp)
{
    json js_src;
    if (!ParseJson(*recv, js_src))
        return;

    if (_waitConId.Exist(conId))
    {
        json js_dest;

        if (!js_src.contains("command") || !js_src["command"].is_number_integer())
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "命令号缺失！";
            resp->QuoteFromBuf(Buffer(js_dest.dump()));
            return;
        }
        int command = js_src.at("command");
        if (command == 999888)
        {
            auto guard1 = _SessionIdToSessionHandle.MakeLockGuard();
            auto guard2 = _ConIdToSessionId.MakeLockGuard();
            std::string sessionId;
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

            auto callback = _CallBackSessionEstablish;
            if (callback)
                std::invoke(callback, JsonProtocolSession(sessionId, this));

            return;
        }
        else
        {
            js_dest["result"] = -1;
            js_dest["reason"] = "无效命令号！";
            resp->QuoteFromBuf(Buffer(js_dest.dump()));
            return;
        }
    }
    else
    {

        std::string sessionId;
        if (!_ConIdToSessionId.FindByLeft(conId, sessionId) || sessionId.empty())
            return;

        std::shared_ptr<SessionHandle> handle;
        if (_SessionIdToSessionHandle.FindByLeft(sessionId, handle) && handle)
        {
            handle->lastActiveTimeSecond = Tool::GetTimestampSecond();
            auto callback = handle->c_request;
            if (callback)
            {
                json js_resp;
                std::invoke(callback, JsonProtocolSession(sessionId, this), js_src, js_resp);
                resp->QuoteFromBuf(Buffer(js_resp.dump()));
            }
        }
    }
}

void JsonProtocolServer::CloseConnect(std::string conId)
{
    if (_waitConId.Exist(conId))
    {
        _waitConId.Erase(conId);
    }
    else
    {

        std::string sessionId;
        if (!_ConIdToSessionId.FindByLeft(conId, sessionId) || sessionId.empty())
            return;

        std::shared_ptr<SessionHandle> handle;
        if (_SessionIdToSessionHandle.FindByLeft(sessionId, handle) && handle)
        {
            auto callback = handle->c_closeconnect;
            if (callback)
                std::invoke(callback, JsonProtocolSession(sessionId, this));
        }
    }
}

void JsonProtocolServer::ConnectionEstablish(std::string conId)
{
    _waitConId.Insert(conId);
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
        std::cout << fmt::format("JsonProtocolServer ParseJson error : {}\n", js_str);
        return false;
    }
}

bool JsonProtocolServer::IsValidSession(const JsonProtocolSession &session)
{
    return !session.getsessionId().empty() && session.getOwner() == this;
}
