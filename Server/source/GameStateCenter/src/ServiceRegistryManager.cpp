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
        if (servicestate->_info->status != info->status)
        {
            std::string str = servicestate->_info->to_json().dump();
            if (info->status == ServiceStatus::OFFLINE)
            {
                std::cout << "Service Offline!" << '\n'
                          << str << '\n';
            }
            else if (info->status == ServiceStatus::DRAINING)
            {
                std::cout << "Service Draining!" << '\n'
                          << str << '\n';
            }
            else if (info->status == ServiceStatus::STARTING)
            {
                std::cout << "Service Starting!" << '\n'
                          << str << '\n';
            }
            else
            {
                std::cerr << "Service Status None!!!" << '\n'
                          << str << '\n';
            }
        }
        servicestate->updateheartbeat();
        servicestate->_info = info;
    }
    else
    {
        servicestate = std::make_shared<ServiceState>(info);
        map.Insert(servicestate->_info->service_id, servicestate);

        {
            std::string str = servicestate->_info->to_json().dump();
            std::cout << "new ServiceConnect!" << '\n'
                      << str << '\n';
        }
    }

    return true;
}

std::vector<ServiceInfo> ServiceRegistryManager::QueryAvailableService(ServiceType type)
{
    std::vector<ServiceInfo> result;
    ServiceStateMap &smap = _service_map[type];

    auto states = smap.GetValues();

    std::sort(states.begin(), states.end(),
              [](const std::shared_ptr<ServiceState> &a, const std::shared_ptr<ServiceState> &b) -> bool
              {
                  return a->_last_heartbeat_time > b->_last_heartbeat_time;
              });

    for (auto state : states)
    {
        if (state->is_alive() && state->_info->status == ServiceStatus::STARTING)
            result.emplace_back(ServiceInfo(*state->_info));
    }

    // smap.EnsureCall(
    //     [&result](std::map<std::string, std::shared_ptr<ServiceState>> &map) -> void
    //     {
    //         for (auto &it : map)
    //         {
    //             auto &servicestate = it.second;
    //             if (servicestate->is_alive() && servicestate->_info->status == ServiceStatus::STARTING)
    //                 result.emplace_back(ServiceInfo(*servicestate->_info));
    //         }
    //     });

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

std::vector<ServiceInfo> ServiceRegistryManager::QueryServiceInfoByServiceIds(const std::vector<std::string> &serviceid_list)
{
    std::vector<ServiceInfo> result;

    for (auto &id : serviceid_list)
    {
        _service_map.EnsureCall(
            [&id, &result](std::map<ServiceType, ServiceStateMap> &service_map) -> void
            {
                for (auto &smap_it : service_map)
                {
                    auto &smap = smap_it.second;
                    smap.EnsureCall(
                        [&id, &result](std::map<std::string, std::shared_ptr<ServiceState>> &map) -> void
                        {
                            for (auto &it : map)
                            {
                                auto &servicestate = it.second;
                                if (servicestate->_info->service_id == id &&
                                    servicestate->is_alive() &&
                                    servicestate->_info->status == ServiceStatus::STARTING)
                                    result.emplace_back(ServiceInfo(*servicestate->_info));
                            }
                        });
                }
            });
    }

    return result;
}
