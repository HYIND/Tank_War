#pragma once

#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "BaseService.h"

using namespace ServiceRegistryDataDef;

struct ServiceSourceHandle
{
    std::weak_ptr<BaseService> weakservicesource;
    std::vector<ServiceInfo> ServiceInfos;
};

class ServiceRegistrar
{
public:
    ServiceRegistrar(uint32_t send_interval = 2000);
    ~ServiceRegistrar();
    bool Start(const std::string &IP, int Port);

    void AddServiceSource(std::shared_ptr<BaseService> source);
    void RemoveServiceSource(std::shared_ptr<BaseService> source);

public:
    void ConnectClose(JsonProtocolClient *session);

private:
    void SendAllServiceInfo();
    void SendServiceInfo(ServiceSourceHandle &handle);

private:
    uint32_t _send_interval_mssecond;
    SafeMap<BaseService *, std::shared_ptr<ServiceSourceHandle>> _sources;

    JsonProtocolClient _client;
    std::shared_ptr<TimerTask> _timer;
};