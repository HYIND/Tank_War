#pragma once

#include "ServiceRegistryManager.h"

// 服务注册表，用于接收服务注册请求
class ServiceDiscovery
{
public:
    ServiceDiscovery();
    ~ServiceDiscovery();
    bool Start(const std::string &IP, int Port);

    void SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m);
    void SetServiceRegistryManager(std::shared_ptr<ServiceRegistryManager> service);

public:
    void OnSessionEstablish(BaseNetWorkSession *session);
    void OnRecvMessage(BaseNetWorkSession *session, json &src);
    void OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest);
    void OnSessionClose(BaseNetWorkSession *session);

private:
    void ProcessMsg(json &src, json &js_dest);
    void ProcessGetServiceInfo(json &js_src, json &js_dest);
    void ProcessGetAllServiceInfo(json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonCommunicateConnectManager> _connectmanager;
    std::weak_ptr<ServiceRegistryManager> _weakserviceregistrymanager;
};