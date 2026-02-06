#include "ApplicationLayerCommunication/JsonProtocolClient.h"

JsonProtocolClient::JsonProtocolClient()
{
    _session = std::make_shared<CustomTcpSession>();
}

JsonProtocolClient::~JsonProtocolClient()
{
    Release();
}

bool JsonProtocolClient::Connect(const std::string &IP, uint16_t Port)
{
    _sessionId = "";
    bool result = _session->Connect(IP, Port);
    if (result)
        result = RequestSessionId();
    if (!result)
        _session->Release();

    if (result)
    {
        _session->BindRecvDataCallBack(std::bind(&JsonProtocolClient::RecvData, this, std::placeholders::_1, std::placeholders::_2));
        _session->BindRecvRequestCallBack(std::bind(&JsonProtocolClient::RecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _session->BindSessionCloseCallBack(std::bind(&JsonProtocolClient::SessionClose, this, std::placeholders::_1));
    }
    return result;
}

bool JsonProtocolClient::Release()
{
    _callbackjsonmessage = nullptr;
    _callbackjsonrequest = nullptr;
    _callbackcloseclient = nullptr;
    return _session->Release();
}

bool JsonProtocolClient::Send(const json &js)
{
    std::string str_resp = js.dump();
    Buffer buf(str_resp);
    return _session->AsyncSend(buf);
}

bool JsonProtocolClient::Request(const json &js_request, json &response)
{
    std::string str_resp = js_request.dump();
    Buffer buf_req(str_resp);
    Buffer buf_resp;
    if (!_session->AwaitSend(buf_req, buf_resp))
        return false;

    response = Tool::ParseJson(buf_resp);
    return true;
}

void JsonProtocolClient::BindCallBackJsonMessage(CallBackJsonMessage &&callback)
{
    _callbackjsonmessage = callback;
}

void JsonProtocolClient::BindCallBackJsonRequest(CallBackJsonRequest &&callback)
{
    _callbackjsonrequest = callback;
}

void JsonProtocolClient::BindCallBackCloseClient(CallBackCloseClient &&callback)
{
    _callbackcloseclient = callback;
}

void JsonProtocolClient::RecvData(BaseNetWorkSession *session, Buffer *recv)
{
    json js = Tool::ParseJson(*recv);
    auto callback = _callbackjsonmessage;
    if (!js.is_null() && callback)
        std::invoke(callback, this, js);
}

void JsonProtocolClient::RecvRequest(BaseNetWorkSession *session, Buffer *recv, Buffer *resp)
{
    json js = Tool::ParseJson(*recv);
    auto callback = _callbackjsonrequest;
    if (!js.is_null() && callback)
    {
        json js_resp;
        std::invoke(callback, this, js, js_resp);
        std::string str_resp = js_resp.dump();
        resp->Write(str_resp.c_str(), str_resp.length());
    }
}

void JsonProtocolClient::SessionClose(BaseNetWorkSession *session)
{
    auto callback = _callbackcloseclient;
    if (callback)
        std::invoke(callback, this);
}

bool JsonProtocolClient::RequestSessionId()
{
    json req, resp;
    req["command"] = 999888;

    if (!Request(req, resp))
        return false;

    if (!resp.contains("result") || !resp["result"].is_number_integer())
        return false;

    int result = resp.value("result", 0);
    if (result < 0)
        return false;

    if (!resp.contains("sessionid") || !resp["sessionid"].is_string())
        return false;

    _sessionId = resp.value("sessionid", "");
    return true;
}
