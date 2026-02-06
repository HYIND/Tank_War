#pragma once

#include "ServiceRegistryManager.h"
#include "ApplicationLayerCommunication/JsonProtocolServer.h"

// 服务注册表，用于接收服务注册请求
class ServiceRegistry
{
public:
    ServiceRegistry();
    ~ServiceRegistry();
    bool Start(const std::string &IP, int Port);

    void SetServiceRegistryManager(std::shared_ptr<ServiceRegistryManager> service);

public:
    void OnSessionEstablish(JsonProtocolSession session);
    void OnRecvMessage(JsonProtocolSession session, json &src);
    void OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    void OnSessionClose(JsonProtocolSession session);

private:
    void ProcessMsg(json &src, json &js_dest);
    void ProcessUpdateServiceInfo(json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonProtocolServer> _connectmanager;
    std::weak_ptr<ServiceRegistryManager> _weakserviceregistrymanager;
};