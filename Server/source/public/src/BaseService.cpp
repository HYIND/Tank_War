#include "BaseService.h"

BaseService::BaseService()
{
    _service_server = std::make_shared<JsonProtocolServer>();
    _stub_server = std::make_shared<JsonProtocolServer>();

    _stub_server->SetCallbackSessionEstablish(std::bind(&BaseService::OnStubSessionEstablish, this, std::placeholders::_1));
}

Task<bool> BaseService::Start()
{
    bool result = _service_enable ? _service_server->Start(_serviceIP, _servicePort) : true;
    bool delegate = _stub_enable ? _stub_server->Start(_stubIP, _stubPort) : true;
    co_return result && delegate;
}

void BaseService::SetServiceEndPoint(const std::string &IP, int Port)
{
    _serviceIP = IP;
    _servicePort = Port;
}

void BaseService::SetStubEndPoint(const std::string &IP, int Port)
{
    _stubIP = IP;
    _stubPort = Port;
}

void BaseService::SetServiceEnable(bool enabled)
{
    _service_enable = enabled;
}

void BaseService::SetStubEnable(bool enabled)
{
    _stub_enable = enabled;
}

bool BaseService::ServiceEnable()
{
    return _service_enable;
}

bool BaseService::StubEnable()
{
    return _stub_enable;
}

Task<void> BaseService::OnStubSessionEstablish(JsonProtocolSession session)
{
    _stub_server->SetCallBackRecvJsonMessage(session, std::bind(&BaseService::OnRecvStubMessage, this, std::placeholders::_1, std::placeholders::_2));
    _stub_server->SetCallBackRecvJsonRequest(session, std::bind(&BaseService::OnRecvStubRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    co_return;
}

Task<void> BaseService::OnRecvStubMessage(JsonProtocolSession session, json &src)
{
    json dest;
    co_await OnStubRequest(src, dest);
    if (!dest.is_null())
        session.AsyncSendJson(dest);
}

Task<void> BaseService::OnRecvStubRequest(JsonProtocolSession session, json &src, json &dest)
{
    co_await OnStubRequest(src, dest);
}

Task<void> BaseService::OnStubRequest(json &js_src, json &js_dest)
{
    co_return;
}
