#include "stdafx.h"
#include "ApplicationLayerConnectManager/JsonCommunicateConnectManager.h"
#include "UserFrontendService.h"
#include "ServiceRegistrar.h"
#include "EndPointConfig.h"
#include "ServiceDiscoveryClient.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        StopNetCoreLoop();
    }
}

auto g_ServiceRegistrar = std::make_shared<ServiceRegistrar>();
auto g_userfrontendservice = std::make_shared<UserFrontendService>();
bool StartService(const std::string &IP, int Port)
{
    auto conn = std::make_shared<JsonCommunicateConnectManager>();
    g_userfrontendservice->SetConnectManager(conn);
    return g_userfrontendservice->Start(IP, Port);
}

bool StartServiceRegistrar(const std::string &IP, int Port)
{
    g_ServiceRegistrar->AddServiceSource(g_userfrontendservice);
    return g_ServiceRegistrar->Start(IP, Port);
}

int main()
{
    // LOGGER->SetLoggerPath("server.log");
    InitNetCore();
    RunNetCoreLoop();

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    {
        if (!StartService(UserFrontendServiceIP, UserFrontendServicePort))
        {
            std::cout << "StartService Error!\n";
            return -1;
        }
    }

    {
        if (!StartServiceRegistrar(ServiceRegistryIP, ServiceRegistryPort))
        {
            std::cout << "StartServiceRegistrar Error!\n";
            return -1;
        }
    }

    {
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } while (NetCoreRunning());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}