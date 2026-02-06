#include "ServiceRegistrar.h"
#include "command.h"

using namespace ServiceRegistryCommand;

ServiceRegistrar::ServiceRegistrar(uint32_t send_interval_mssecond)
    : _send_interval_mssecond(send_interval_mssecond)
{
    if (_send_interval_mssecond <= 20)
        _send_interval_mssecond = 20;
}

ServiceRegistrar::~ServiceRegistrar()
{
    if (_timer)
    {
        _timer->Clean();
        _timer.reset();
    }
    _client.Release();
}

bool ServiceRegistrar::Start(const std::string &IP, int Port)
{
    _client.Release();
    if (_timer)
    {
        _timer->Clean();
        _timer.reset();
    }

    _client.BindCallBackCloseClient(std::bind(&ServiceRegistrar::ConnectClose, this, std::placeholders::_1));
    bool success = _client.Connect(IP, Port);
    if (success)
    {
        _timer = TimerTask::CreateRepeat("ServiceRegistrar_SendServiceInfo", _send_interval_mssecond, std::bind(&ServiceRegistrar::SendAllServiceInfo, this), 0);
        _timer->Run();
    }

    return success;
}

void ServiceRegistrar::AddServiceSource(std::shared_ptr<BaseService> source)
{
    auto handle = std::make_shared<ServiceSourceHandle>();
    handle->weakservicesource = source;
    _sources[source.get()] = handle;
}

void ServiceRegistrar::RemoveServiceSource(std::shared_ptr<BaseService> source)
{
    auto guard = _sources.MakeLockGuard();
    if (_sources.Exist(source.get()))
    {
        auto &handle = _sources[source.get()];
        for (auto &serviceinfo : handle->ServiceInfos)
            serviceinfo.status = ServiceStatus::OFFLINE;
        SendServiceInfo(*handle);
        _sources.Erase(source.get());
    }
}

void ServiceRegistrar::ConnectClose(JsonProtocolClient *session)
{
    if (_timer)
    {
        _timer->Clean();
        _timer.reset();
    }
}

void ServiceRegistrar::SendAllServiceInfo()
{
    _sources.EnsureCall(
        [&](std::map<BaseService *, std::shared_ptr<ServiceSourceHandle>> &map) -> void
        {
            for (auto it = map.begin(); it != map.end();)
            {
                auto handle = it->second;
                if (auto servicesource = handle->weakservicesource.lock())
                {
                    try
                    {
                        handle->ServiceInfos = servicesource->GetServiceInfo();
                        SendServiceInfo(*handle);
                    }
                    catch (...)
                    {
                        std::cout << "ServiceRegistrar GetServiceInfo Execption!\n";
                    }
                    it++;
                }
                else
                {
                    for (auto &serviceinfo : handle->ServiceInfos)
                        serviceinfo.status = ServiceStatus::OFFLINE;
                    SendServiceInfo(*handle);
                    it = map.erase(it);
                }
            }
        });
}

void ServiceRegistrar::SendServiceInfo(ServiceSourceHandle &handle)
{
    for (auto &serviceinfo : handle.ServiceInfos)
    {
        json js_info;
        js_info["command"] = ServiceRegistry_UpdateServiceInfo;
        js_info["service"] = serviceinfo.to_json();

        std::string str = js_info.dump();
        if (!_client.Send(js_info))
        {
            _client.Release();
            ConnectClose(&_client);
        }
    }
}
