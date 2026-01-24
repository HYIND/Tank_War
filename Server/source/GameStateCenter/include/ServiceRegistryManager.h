#pragma once

#include "ServiceRegistryData.h"
#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"
#include <memory>

using namespace ServiceRegistryDataDef;
struct ServiceState
{
    std::shared_ptr<ServiceInfo> _info;

    int64_t _register_time;
    int64_t _last_heartbeat_time;

    std::chrono::seconds _timeout;

    ServiceState(std::shared_ptr<ServiceInfo> info, std::chrono::seconds timeout = std::chrono::seconds(30));
    void updateheartbeat();
    bool is_alive() const;
};

// 服务注册
class ServiceRegistryManager
{
    using ServiceStateMap = SafeMap<std::string, std::shared_ptr<ServiceState>>; // name->ServiceState

public:
    ServiceRegistryManager();
    ~ServiceRegistryManager();

public:
    bool UpdateService(json &js);
    std::vector<ServiceInfo> QueryAvailableService(ServiceType type);
    std::vector<ServiceInfo> QueryAllAvailableService();

private:
    SafeMap<ServiceType, ServiceStateMap> _service_map;
};