#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"

JsonCommunicateConnectManager::JsonCommunicateConnectManager()
{
    _sessionmanager.SetCallBackRecvMessage(std::bind(&JsonCommunicateConnectManager::RecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _sessionmanager.SetCallBackRequestMessage(std::bind(&JsonCommunicateConnectManager::RequestMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _sessionmanager.SetCallBackSessionEstablish(std::bind(&JsonCommunicateConnectManager::SessionEstablish, this, std::placeholders::_1));
    _sessionmanager.SetCallBackCloseConnect(std::bind(&JsonCommunicateConnectManager::CloseConnect, this, std::placeholders::_1));
}

bool JsonCommunicateConnectManager::Start(const std::string &IP, int Port)
{
    return _sessionmanager.Start(IP, Port);
}

void JsonCommunicateConnectManager::SetCallBackRecvJsonMessage(BaseNetWorkSession *session, CallBackRecvJsonMessage &&callback)
{
    _CallBackRecvJsonMessage_Map[session] = callback;
}

void JsonCommunicateConnectManager::SetCallBackRecvJsonRequest(BaseNetWorkSession *session, CallBackRecvJsonRequest &&callback)
{
    _CallBackRecvJsonRequest_Map[session] = callback;
}

void JsonCommunicateConnectManager::SetCallBackCloseConnect(BaseNetWorkSession *session, CallBackCloseConnect &&callback)
{
    _CallBackCloseConnect_Map[session] = callback;
}

void JsonCommunicateConnectManager::SetCallbackSessionEstablish(CallBackSessionEstablish &&callback)
{
    _CallBackSessionEstablish = callback;
}

void JsonCommunicateConnectManager::RecvMessage(BaseNetWorkSession *session, Buffer *recv)
{
    json js_src;
    if (!ParseJson(*recv, js_src))
        return;

    CallBackRecvJsonMessage callback;
    if (_CallBackRecvJsonMessage_Map.Find(session, callback) && callback)
    {
        std::invoke(callback, session, js_src);
    }
}

void JsonCommunicateConnectManager::RequestMessage(BaseNetWorkSession *session, Buffer *recv, Buffer *resp)
{
    json js_src;
    if (!ParseJson(*recv, js_src))
        return;

    json js_resp;
    CallBackRecvJsonRequest callback;
    if (_CallBackRecvJsonRequest_Map.Find(session, callback) && callback)
    {
        std::invoke(callback, session, js_src, js_resp);
        resp->QuoteFromBuf(Buffer(js_resp.dump()));
    }
}

void JsonCommunicateConnectManager::CloseConnect(BaseNetWorkSession *session)
{
    CallBackCloseConnect callback;
    if (_CallBackCloseConnect_Map.Find(session, callback) && callback)
        std::invoke(callback, session);
}

void JsonCommunicateConnectManager::SessionEstablish(BaseNetWorkSession *session)
{
    auto callback = _CallBackSessionEstablish;
    if (callback)
        std::invoke(callback, session);
}

bool JsonCommunicateConnectManager::ParseJson(Buffer &buf, json &js)
{
    std::string js_str(buf.Byte(), buf.Length());
    try
    {
        js = json::parse(js_str);
        return true;
    }
    catch (...)
    {
        std::cout << fmt::format("JsonCommunicateConnectManager ParseJson error : {}\n", js_str);
        return false;
    }
}
