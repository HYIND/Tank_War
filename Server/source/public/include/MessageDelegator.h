#pragma once

#include "GameStateDef/UserDef.h"
#include "ServiceDiscoveryClient.h"
#include "ServiceRegistryManager.h"
#include "BaseService.h"

struct LocalServiceHandle
{
    std::string serviceid;
    ServiceType type;
    std::weak_ptr<BaseService> weakservice;
};

class MessageDelegator
{
public:
    MessageDelegator();
    ~MessageDelegator();

    void SetGameStateServiceEndpoint(const std::string &IP, int Port);
    void SetServiceDiscoveryEndpoint(const std::string &IP, int Port);

    void AddLocalService(std::weak_ptr<BaseService> service, const std::string &serviceid, ServiceType type);
    void SetSkipLocalService(bool enabled);
    bool SkipLocalService();

public:
    bool DelegateMessage(const std::string &goaltoken, const json &js);
    bool DelegateMessage(const std::string &goaltoken, ServiceType type, const json &js);

private:
    bool ConnectGameStateService();
    bool RequestGameStateService(json &src, json &dest);
    bool GetAvailableServiceInfoForUser(const std::string &goaltoken, std::vector<ServiceInfo> &services);

    bool GetServiceIdsUserConnected(const std::string &goaltoken, std::vector<GameStateDef::UserConnectedServiceInfo> &infos);

    bool RequestDelegateToServices(std::vector<ServiceInfo> &services, const std::string &goaltoken, const json &js);

private:
    std::string _gameStateServiceIP;
    int _gameStateServicePort;

    JsonProtocolClient _gameStateServiceClient;
    std::atomic<bool> isGameStateServiceConnected;
    CriticalSectionLock _sessionStatusChangeLock;

    ServiceDiscoveryClient _serviceDiscoverClient;

    SafeMap<std::string, LocalServiceHandle> _localService;
    bool _skipLocalService  = false;
};