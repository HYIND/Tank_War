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

Task<bool> MessageDelegator::DelegateMessage(const std::string &goaltoken, const json &js)
{
    if (!co_await ConnectGameStateService())
        co_return false;

    std::vector<ServiceInfo> services;
    if (!co_await GetAvailableServiceInfoForUser(goaltoken, services))
        co_return false;

    co_return !co_await RequestDelegateToServices(services, goaltoken, js);
}

Task<bool> MessageDelegator::DelegateMessage(const std::string &goaltoken, ServiceType type, const json &js)
{
    if (!co_await ConnectGameStateService())
        co_return false;

    std::vector<ServiceInfo> services;
    if (!co_await GetAvailableServiceInfoForUser(goaltoken, services))
        co_return false;

    co_return !co_await RequestDelegateToServices(services, goaltoken, js);
}

Task<bool> MessageDelegator::GetAvailableServiceInfoForUser(const std::string &goaltoken, std::vector<ServiceInfo> &services)
{
    if (!co_await ConnectGameStateService())
        co_return false;

    std::vector<GameStateDef::UserConnectedServiceInfo> infos;
    if (!co_await GetServiceIdsUserConnected(goaltoken, infos))
        co_return false;

    std::vector<std::string> serviceids;
    for (auto &info : infos)
        serviceids.emplace_back(info.service_id);

    if (!co_await _serviceDiscoverClient.GetAvailableServiceInfoByServicesIds(serviceids, services))
        co_return false;

    co_return true;
}

Task<bool> MessageDelegator::GetServiceIdsUserConnected(const std::string &goaltoken, std::vector<GameStateDef::UserConnectedServiceInfo> &infos)
{

    json js_request;
    js_request["command"] = GameStateService_FindUserEndPoint;
    js_request["token"] = goaltoken;

    json js_response;
    if (!co_await RequestGameStateService(js_request, js_response) || js_response.is_null())
        co_return false;

    if (!js_response.contains("result") || !js_response["result"].is_array())
        co_return false;

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

    co_return true;
}

Task<bool> MessageDelegator::RequestDelegateToServices(std::vector<ServiceInfo> &services, const std::string &goaltoken, const json &js)
{
    if (services.empty())
        co_return true;

    json delegate_req;
    delegate_req["command"] = MessageDelegate_UserMessageDelegate;
    delegate_req["delegate_token"] = goaltoken;
    delegate_req["content"] = js;

    auto sendDelegate = [&](const std::string &ip, int port) -> Task<bool>
    {
        JsonProtocolClient client;
        if (!co_await client.Connect(ip, port))
            co_return false;

        json response;
        if (!co_await client.Request(delegate_req, response))
            co_return false;

        co_return true;
    };

    auto tryFindServiceLocal = [&](const std::string &serviceid) -> Task<bool>
    {
        LocalServiceHandle handle;
        if (_localService.Find(serviceid, handle))
        {
            auto service = handle.weakservice.lock();
            if (service)
            {
                if (_skipLocalService)
                    co_return true;
                json resp;
                co_await service->OnStubRequest(delegate_req, resp);
                co_return true;
            }
            else
                co_return false;
        }
        co_return false;
    };

    bool result = true;
    for (auto &service : services)
    {
        if (co_await tryFindServiceLocal(service.service_id))
            continue;
        else
        {
            if (service.stub_endpoint.ip.empty() || service.stub_endpoint.port == 0)
                continue;
            if (!co_await sendDelegate(service.stub_endpoint.ip, service.stub_endpoint.port) && result)
                result = false;
        }
    }

    co_return result;
}

Task<bool> MessageDelegator::ConnectGameStateService()
{
    if (!isGameStateServiceConnected.load())
    {
        LockGuard guard(_sessionStatusChangeLock);
        if (!isGameStateServiceConnected.load())
        {
            _gameStateServiceClient.Release();
            isGameStateServiceConnected = co_await _gameStateServiceClient.Connect(_gameStateServiceIP, _gameStateServicePort);
        }
    }
    co_return isGameStateServiceConnected;
}

Task<bool> MessageDelegator::RequestGameStateService(json &src, json &dest)
{
    co_return co_await _gameStateServiceClient.Request(src, dest);
}
