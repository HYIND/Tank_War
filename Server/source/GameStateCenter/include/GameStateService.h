#pragma once

#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"
#include "BaseService.h"
#include "GameStateManager.h"

using namespace ServiceRegistryDataDef;

class GameStateService : public BaseService
{

public:
    GameStateService();
    ~GameStateService();
    bool Start(const std::string &IP, int Port);

    void SetConnectManager(std::shared_ptr<JsonCommunicateConnectManager> m);
    void SetGameStateManager(std::shared_ptr<GameStateManager> gsm);

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
    std::weak_ptr<GameStateManager> _GSM;
    std::shared_ptr<ServiceInfo> _serviceinfo;
    std::shared_ptr<JsonCommunicateConnectManager> _connectmanager;
};