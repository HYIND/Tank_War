#pragma once

#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ServiceRegistryData.h"

using namespace ServiceRegistryDataDef;

class BaseService
{
public:
    BaseService();

    virtual Task<bool> Start();
    void SetServiceEndPoint(const std::string &IP, int Port);
    void SetStubEndPoint(const std::string &IP, int Port);

    void SetServiceEnable(bool enabled);
    void SetStubEnable(bool enabled);

    bool ServiceEnable();
    bool StubEnable();

public:
    Task<void> OnStubSessionEstablish(JsonProtocolSession session);
    Task<void> OnRecvStubMessage(JsonProtocolSession session, json &src);
    Task<void> OnRecvStubRequest(JsonProtocolSession session, json &src, json &dest);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo() = 0;
    virtual Task<void> OnStubRequest(json &js_src, json &js_dest);

protected:
    bool _service_enable = true;
    std::string _serviceIP;
    int _servicePort;
    std::shared_ptr<JsonProtocolServer> _service_server;

    bool _stub_enable = true;
    std::string _stubIP;
    int _stubPort;
    std::shared_ptr<JsonProtocolServer> _stub_server;
};