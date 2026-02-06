#include "MessageDelegator.h"
#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "command.h"

using namespace ServiceDiscoveryCommand;
using namespace MessageDelegateCommand;
using namespace GameStateServiceCommand;

MessageDelegator::MessageDelegator()
    : isGameStateServiceConnected{false}
{
}

MessageDelegator::~MessageDelegator()
{
    LockGuard guard(_sessionStatusChangeLock);
    isGameStateServiceConnected.store(false);
    _gameStateServiceClient.Release();
}

void MessageDelegator::SetGameStateServiceEndpoint(const std::string &IP, int Port)
{
    if (_gameStateServicePort == Port && _gameStateServiceIP == IP)
        return;

    LockGuard guard(_sessionStatusChangeLock);
    isGameStateServiceConnected.store(false);
    _gameStateServiceIP = IP;
    _gameStateServicePort = Port;
    _gameStateServiceClient.Release();
}

void MessageDelegator::SetServiceDiscoveryEndpoint(const std::string &IP, int Port)
{
    _serviceDiscoverClient.SetEndpoint(IP, Port);
}

void MessageDelegator::AddLocalService(std::weak_ptr<BaseService> service, const std::string &serviceid, ServiceType type)
{
    LocalServiceHandle handle;
    handle.serviceid = serviceid;
    handle.type = type;
    handle.weakservice = service;
    _localService[serviceid] = std::move(handle);
}

void MessageDelegator::SetSkipLocalService(bool enabled)
{
    _skipLocalService = enabled;
}

bool MessageDelegator::SkipLocalService()
{
    return _skipLocalService;
}

bool MessageDelegator::DelegateMessage(const std::string &goaltoken, const json &js)
{
    if (!ConnectGameStateService())
        return false;

    std::vector<ServiceInfo> services;
    if (!GetAvailableServiceInfoForUser(goaltoken, services))
        return false;

    return !RequestDelegateToServices(services, goaltoken, js);
}

bool MessageDelegator::DelegateMessage(const std::string &goaltoken, ServiceType type, const json &js)
{
    if (!ConnectGameStateService())
        return false;

    std::vector<ServiceInfo> services;
    if (!GetAvailableServiceInfoForUser(goaltoken, services))
        return false;

    return !RequestDelegateToServices(services, goaltoken, js);
}

bool MessageDelegator::GetAvailableServiceInfoForUser(const std::string &goaltoken, std::vector<ServiceInfo> &services)
{
    if (!ConnectGameStateService())
        return false;

    std::vector<GameStateDef::UserConnectedServiceInfo> infos;
    if (!GetServiceIdsUserConnected(goaltoken, infos))
        return false;

    std::vector<std::string> serviceids;
    for (auto &info : infos)
        serviceids.emplace_back(info.service_id);

    if (!_serviceDiscoverClient.GetAvailableServiceInfoByServicesIds(serviceids, services))
        return false;

    return true;
}

bool MessageDelegator::GetServiceIdsUserConnected(const std::string &goaltoken, std::vector<GameStateDef::UserConnectedServiceInfo> &infos)
{

    json js_request;
    js_request["command"] = GameStateService_FindUserEndPoint;
    js_request["token"] = goaltoken;

    json js_response;
    if (!RequestGameStateService(js_request, js_response) || js_response.is_null())
        return false;

    if (!js_response.contains("result") || !js_response["result"].is_array())
        return false;

    json js_result = js_response["result"];
    for (int i = 0; i < js_result.size(); i++)
    {
        json js_info = js_result.at(i);
        if (js_info.contains("id") && js_info.contains("type") && js_info["id"].is_string() && js_info["type"].is_number_unsigned())
        {
            GameStateDef::UserConnectedServiceInfo info;
            info.service_id = js_info["id"];
            info.service_type = (ServiceRegistryDataDef::ServiceType)js_info["type"];
            infos.emplace_back(std::move(info));
        }
    }

    return true;
}

bool MessageDelegator::RequestDelegateToServices(std::vector<ServiceInfo> &services, const std::string &goaltoken, const json &js)
{
    if (services.empty())
        return true;

    json delegate_req;
    delegate_req["command"] = MessageDelegate_UserMessageDelegate;
    delegate_req["delegate_token"] = goaltoken;
    delegate_req["content"] = js;

    auto sendDelegate = [&](const std::string &ip, int port) -> bool
    {
        JsonProtocolClient client;
        if (!client.Connect(ip, port))
            return false;

        json response;
        if (!client.Request(delegate_req, response))
            return false;

        return true;
    };

    auto tryFindServiceLocal = [&](const std::string &serviceid) -> bool
    {
        LocalServiceHandle handle;
        if (_localService.Find(serviceid, handle))
        {
            auto service = handle.weakservice.lock();
            if (service)
            {
                if (_skipLocalService)
                    return true;
                json resp;
                service->OnStubRequest(delegate_req, resp);
                return true;
            }
            else
                return false;
        }
        return false;
    };

    bool result = true;
    for (auto &service : services)
    {
        if (tryFindServiceLocal(service.service_id))
            continue;
        else
        {
            if (service.stub_endpoint.ip.empty() || service.stub_endpoint.port == 0)
                continue;
            if (!sendDelegate(service.stub_endpoint.ip, service.stub_endpoint.port) && result)
                result = false;
        }
    }

    return result;
}

bool MessageDelegator::ConnectGameStateService()
{
    if (!isGameStateServiceConnected.load())
    {
        LockGuard guard(_sessionStatusChangeLock);
        if (!isGameStateServiceConnected.load())
        {
            _gameStateServiceClient.Release();
            isGameStateServiceConnected = _gameStateServiceClient.Connect(_gameStateServiceIP, _gameStateServicePort);
        }
    }
    return isGameStateServiceConnected;
}

bool MessageDelegator::RequestGameStateService(json &src, json &dest)
{
    return _gameStateServiceClient.Request(src, dest);
}
