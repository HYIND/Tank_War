#pragma once

#include "ServiceRegistryManager.h"

// 服务注册表，用于接收服务注册请求
class ServiceDiscoveryClient
{
public:
    ServiceDiscoveryClient(const std::string &IP, int Port);
    ~ServiceDiscoveryClient();

    void SetAddr(const std::string &IP);
    void SetPort(int Port);
    void SetEndpoint(const std::string &IP, int Port);

    bool GetAvailableServiceInfo(ServiceType type, std::vector<ServiceInfo> &services);
    bool GetAllAvailableServiceInfo(std::vector<ServiceInfo> &services);

private:
    bool Connect();
    bool Request(json &src, json &dest);

private:
    std::string _ip;
    int _port;

    CustomTcpSession _tcpsession;
    std::atomic<bool> isConnected;
    CriticalSectionLock _sessionStatusChangeLock;
};