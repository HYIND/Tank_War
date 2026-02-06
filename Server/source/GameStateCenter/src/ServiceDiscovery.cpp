#include "ServiceDiscovery.h"
#include "command.h"

using namespace ServiceRegistryDataDef;
using namespace ServiceDiscoveryCommand;

ServiceDiscovery::ServiceDiscovery()
{
    _connectmanager = std::make_shared<JsonProtocolServer>();
    _connectmanager->SetCallbackSessionEstablish(std::bind(&ServiceDiscovery::OnSessionEstablish, this, std::placeholders::_1));
}

ServiceDiscovery::~ServiceDiscovery()
{
}

bool ServiceDiscovery::Start(const std::string &IP, int Port)
{
    return _connectmanager->Start(IP, Port);
}

void ServiceDiscovery::OnSessionEstablish(JsonProtocolSession session)
{
    _connectmanager->SetCallBackRecvJsonMessage(session, std::bind(&ServiceDiscovery::OnRecvMessage, this, std::placeholders::_1, std::placeholders::_2));
    _connectmanager->SetCallBackRecvJsonRequest(session, std::bind(&ServiceDiscovery::OnRecvRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _connectmanager->SetCallBackCloseConnect(session, std::bind(&ServiceDiscovery::OnSessionClose, this, std::placeholders::_1));
}

void ServiceDiscovery::OnRecvMessage(JsonProtocolSession session, json &src)
{
    json dest;
    ProcessMsg(src, dest);
    if (!dest.is_null())
    {
        session.AsyncSendJson(dest);
    }
}

void ServiceDiscovery::OnRecvRequest(JsonProtocolSession session, json &src, json &dest)
{
    ProcessMsg(src, dest);
}

void ServiceDiscovery::OnSessionClose(JsonProtocolSession session)
{
}

void ServiceDiscovery::SetServiceRegistryManager(std::shared_ptr<ServiceRegistryManager> service)
{
    _weakserviceregistrymanager = std::weak_ptr<ServiceRegistryManager>(service);
}

void ServiceDiscovery::ProcessMsg(json &js_src, json &js_dest)
{
    if (!js_src.contains("command"))
        return;
    int command = js_src.at("command");

    if (command == ServiceDiscovery_GetServiceInfo)
    {
        ProcessGetServiceInfo(js_src, js_dest);
    }
    else if (command == ServiceDiscovery_GetAllServiceInfo)
    {
        ProcessGetAllServiceInfo(js_src, js_dest);
    }
    else if (command == ServiceDiscovery_GetServiceInfoByServiceIds)
    {
        ProcessGetServiceInfoByServiceIds(js_src, js_dest);
    }
}

void ServiceDiscovery::ProcessGetServiceInfo(json &js_src, json &js_dest)
{
    js_dest["command"] = ServiceDiscovery_GetServiceInfoRes;
    json js_services = json::array();

    int type = js_src.value("type", -1);
    if (type <= (int)ServiceType::MinValue || type >= (int)ServiceType::MaxValue)
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "Error ServiceType";
        js_dest["services"] = js_services;
        return;
    }

    ServiceType servicetype = (ServiceType)type;

    auto manager = _weakserviceregistrymanager.lock();
    if (manager)
    {
        std::vector<ServiceInfo> servicestates = manager->QueryAvailableService(servicetype);
        for (auto &info : servicestates)
        {
            js_services.push_back(info.to_json());
        }
        js_dest["result"] = 1;
        js_dest["services"] = js_services;
        return;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误";
        js_dest["services"] = js_services;
        return;
    }
}

void ServiceDiscovery::ProcessGetAllServiceInfo(json &js_src, json &js_dest)
{
    js_dest["command"] = ServiceDiscovery_GetServiceInfoRes;

    json js_services = json::array();

    auto manager = _weakserviceregistrymanager.lock();
    if (manager)
    {
        std::vector<ServiceInfo> servicestates = manager->QueryAllAvailableService();
        for (auto &info : servicestates)
        {
            js_services.push_back(info.to_json());
        }
        js_dest["result"] = 1;
        js_dest["services"] = js_services;
        return;
    }
    else
    {
        js_dest["result"] = -1;
        js_dest["reason"] = "服务器内部错误";
        js_dest["services"] = js_services;
        return;
    }
}

void ServiceDiscovery::ProcessGetServiceInfoByServiceIds(json &js_src, json &js_dest)
{
    js_dest["command"] = ServiceDiscovery_GetServiceInfoByServiceIdsRes;

    bool success = true;
    std::string errno_reason;

    std::vector<std::string> serviceid_list;
    if (success)
    {
        if (!js_src["serviceids"].is_array())
        {
            success = false;
            errno_reason = "服务器内部错误";
        }
    }

    if (success)
    {
        json js_serviceids = js_src["serviceids"];
        for (int i = 0; i < js_serviceids.size(); i++)
        {
            std::string id = js_serviceids.at(i);
            if (id.empty())
                continue;
            serviceid_list.emplace_back(std::move(id));
        }
    }

    auto manager = _weakserviceregistrymanager.lock();
    if (success)
    {

        if (!manager)
        {
            success = false;
            errno_reason = "服务器内部错误";
        }
    }

    json js_services = json::array();
    if (success)
    {
        std::vector<ServiceInfo> servicestates = manager->QueryServiceInfoByServiceIds(serviceid_list);
        for (auto &info : servicestates)
        {
            js_services.push_back(info.to_json());
        }
    }

    if (success)
    {
        js_dest["result"] = 1;
        js_dest["services"] = js_services;
    }
    else
    {

        js_dest["result"] = -1;
        js_dest["reason"] = errno_reason;
    }
}
