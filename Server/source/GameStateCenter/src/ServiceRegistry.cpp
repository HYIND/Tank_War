#include "ServiceRegistry.h"
#include "command.h"

using namespace ServiceRegistryCommand;

ServiceRegistry::ServiceRegistry()
{
    _connectmanager = std::make_shared<JsonProtocolServer>();
    _connectmanager->SetCallbackSessionEstablish(std::bind(&ServiceRegistry::OnSessionEstablish, this, std::placeholders::_1));
}

ServiceRegistry::~ServiceRegistry()
{
}

bool ServiceRegistry::Start(const std::string &IP, int Port)
{
    return _connectmanager->Start(IP, Port);
}

void ServiceRegistry::SetServiceRegistryManager(std::shared_ptr<ServiceRegistryManager> service)
{
    _weakserviceregistrymanager = std::weak_ptr<ServiceRegistryManager>(service);
}

Task<void> ServiceRegistry::OnSessionEstablish(JsonProtocolSession session)
{
    _connectmanager->SetCallBackRecvJsonMessage(session, std::bind(&ServiceRegistry::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _connectmanager->SetCallBackRecvJsonRequest(session, std::bind(&ServiceRegistry::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _connectmanager->SetCallBackCloseConnect(session, std::bind(&ServiceRegistry::OnSessionClose, this, std::placeholders::_1));
    co_return;
}

Task<void> ServiceRegistry::OnRecvMessage(JsonProtocolSession session, json &src)
{
    json dest;
    co_await ProcessMsg(src, dest);
    if (!dest.is_null())
        session.AsyncSendJson(dest);;
    co_return;
}

Task<void> ServiceRegistry::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    co_await ProcessMsg(src, dest);
}

Task<void> ServiceRegistry::OnSessionClose(JsonProtocolSession session)
{
    co_return;
}

Task<void> ServiceRegistry::ProcessMsg(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        co_return;
    int command = js_src.at("command");

    if (command == ServiceRegistry_UpdateServiceInfo)
    {
        co_await ProcessUpdateServiceInfo(js_src, js_dest);
    }
}

Task<void> ServiceRegistry::ProcessUpdateServiceInfo(json &js_src, json &js_dest)
{
    js_dest["command"] = ServiceRegistry_UpdateServiceInfoRes;

    if (!js_src.contains("service") || !js_src["service"].is_object())
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "未找到Service信息";
        co_return;
    }

    auto manager = _weakserviceregistrymanager.lock();
    if (manager && manager->UpdateService(js_src["service"]))
    {
        js_dest["result"] = 1;
        co_return;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误";
        co_return;
    }
}
