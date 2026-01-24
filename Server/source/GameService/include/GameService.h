#pragma once

#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"
#include "BaseService.h"

using namespace ServiceRegistryDataDef;

class GameService : public BaseService
{

public:
    GameService();
    ~GameService();
    bool Start(const std::string &IP, int Port);

    void SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m);

public:
    void OnSessionEstablish(BaseNetWorkSession *session);
    void OnRecvMessage(BaseNetWorkSession *session, json &src);
    void OnRecvRequest(BaseNetWorkSession *session, json &src, json &dest);
    void OnSessionClose(BaseNetWorkSession *session);

public:
    virtual std::vector<ServiceInfo> GetServiceInfo();

private:
    void ProcessMsg(json &js_src, json &js_dest);

private:
    std::shared_ptr<ServiceInfo> _serviceinfo;
    std::shared_ptr<JsonCommunicateConnectManager> _connectmanager;
};