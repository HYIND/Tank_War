#pragma once

#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "ServiceRegistryData.h"

using namespace ServiceRegistryDataDef;

// 服务发现客户端
class ServiceDiscoveryClient
{
public:
    ServiceDiscoveryClient();
    ServiceDiscoveryClient(const std::string &IP, int Port);
    ~ServiceDiscoveryClient();

    void SetAddr(const std::string &IP);
    void SetPort(int Port);
    void SetEndpoint(const std::string &IP, int Port);

    bool GetAvailableServiceInfo(ServiceType type, std::vector<ServiceInfo> &services);
    bool GetAllAvailableServiceInfo(std::vector<ServiceInfo> &services);
    bool GetAvailableServiceInfoByServicesIds(std::vector<std::string> &serviceid_list, std::vector<ServiceInfo> &services);

private:
    bool Connect();
    bool Request(json &src, json &dest);

private:
    std::string _ip;
    int _port;

    JsonProtocolClient _client;
    std::atomic<bool> isConnected;
    CriticalSectionLock _sessionStatusChangeLock;
};