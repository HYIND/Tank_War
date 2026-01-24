#include "ServiceRegistryManager.h"

ServiceState::ServiceState(std::shared_ptr<ServiceInfo> info, std::chrono::seconds timeout)
    : _info(std::move(info)),
      _register_time(Tool::GetTimestampMilliseconds()),
      _last_heartbeat_time(Tool::GetTimestampMilliseconds()),
      _timeout(timeout)
{
}

void ServiceState::updateheartbeat()
{
    _last_heartbeat_time = Tool::GetTimestampMilliseconds();
}

bool ServiceState::is_alive() const
{
    if (_info->status == ServiceStatus::OFFLINE)
        return false;
    int64_t now = Tool::GetTimestampMilliseconds();
    int64_t diff = now - _last_heartbeat_time;
    int64_t timeoutms = _timeout.count() * 1000;
    return diff < timeoutms;
}

ServiceRegistryManager::ServiceRegistryManager()
{
}

ServiceRegistryManager::~ServiceRegistryManager()
{
}

bool ServiceRegistryManager::UpdateService(json &recv)
{
    auto info = ServiceInfo::create_from_json_shared(recv);
    if (info->service_id.empty())
        return false;

    ServiceStateMap &map = _service_map[info->service_type];

    std::shared_ptr<ServiceState> servicestate;
    if (map.Find(info->service_id, servicestate))
    {
        servicestate->updateheartbeat();
        servicestate->_info = info;
    }
    else
    {
        servicestate = std::make_shared<ServiceState>(info);
        map.Insert(servicestate->_info->service_id, servicestate);

        {
            std::string str = servicestate->_info->to_json().dump();
            std::cout << "newServiceConnect!" << str << "\n";
        }
    }

    return true;
}

std::vector<ServiceInfo> ServiceRegistryManager::QueryAvailableService(ServiceType type)
{
    std::vector<ServiceInfo> result;
    ServiceStateMap &smap = _service_map[type];

    smap.EnsureCall(
        [&result](std::map<std::string, std::shared_ptr<ServiceState>> &map) -> void
        {
            for (auto &it : map)
            {
                auto &servicestate = it.second;
                if (servicestate->is_alive() && servicestate->_info->status == ServiceStatus::STARTING)
                    result.emplace_back(ServiceInfo(*servicestate->_info));
            }
        });

    return result;
}

std::vector<ServiceInfo> ServiceRegistryManager::QueryAllAvailableService()
{
    std::vector<ServiceInfo> result;

    _service_map.EnsureCall(
        [&result](std::map<ServiceType, ServiceStateMap> &service_map) -> void
        {
            for (auto &smap_it : service_map)
            {
                auto &smap = smap_it.second;
                smap.EnsureCall(
                    [&result](std::map<std::string, std::shared_ptr<ServiceState>> &map) -> void
                    {
                        for (auto &it : map)
                        {
                            auto &servicestate = it.second;
                            if (servicestate->is_alive() && servicestate->_info->status == ServiceStatus::STARTING)
                                result.emplace_back(ServiceInfo(*servicestate->_info));
                        }
                    });
            }
        });

    return result;
}
