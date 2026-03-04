#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ServiceRegistryManager.h"

// 用于对外提供服务发现
class ServiceDiscovery
{
public:
    ServiceDiscovery();
    ~ServiceDiscovery();
    bool Start(const std::string &IP, int Port);

    void SetServiceRegistryManager(std::shared_ptr<ServiceRegistryManager> service);

public:
    Task<void> OnSessionEstablish(JsonProtocolSession session);
    Task<void> OnRecvMessage(JsonProtocolSession session, json &src);
    Task<void> OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    Task<void> OnSessionClose(JsonProtocolSession session);

private:
    Task<void> ProcessMsg(json &src, json &js_dest);
    Task<void> ProcessGetServiceInfo(json &js_src, json &js_dest);
    Task<void> ProcessGetAllServiceInfo(json &js_src, json &js_dest);
    Task<void> ProcessGetServiceInfoByServiceIds(json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonProtocolServer> _connectmanager;
    std::weak_ptr<ServiceRegistryManager> _weakserviceregistrymanager;
};