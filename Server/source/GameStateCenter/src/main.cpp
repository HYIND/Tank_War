#include "stdafx.h"
#include "ServiceRegistryManager.h"
#include "ServiceRegistry.h"
#include "GameStateManager.h"
#include "GameStateService.h"
#include "ServiceRegistrar.h"
#include "ServiceDiscovery.h"
#include "ServiceDiscoveryClient.h"
#include "EndPointConfig.h"

auto g_GSM = std::make_shared<GameStateManager>();
auto g_GSS = std::make_shared<GameStateService>();
auto g_SRr = std::make_shared<ServiceRegistrar>();

auto g_SRM = std::make_shared<ServiceRegistryManager>();
auto g_SRy = std::make_shared<ServiceRegistry>();

auto g_SD = std::make_shared<ServiceDiscovery>();

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        g_SRr.reset();
        StopNetCoreLoop();
    }
}

bool StartServiceRegistry(const std::string &IP, int Port)
{
    g_SRy->SetServiceRegistryManager(g_SRM);
    return g_SRy->Start(IP, Port);
}

bool StartServiceDiscovery(const std::string &IP, int Port)
{
    g_SD->SetServiceRegistryManager(g_SRM);
    return g_SD->Start(IP, Port);
}

bool StartService(const std::string &IP, int Port)
{
    g_GSS->SetStubEndPoint(IP, Port);
    g_GSS->SetGameStateManager(g_GSM);
    return g_GSS->Start();
}

bool StartServiceRegistrar(const std::string &IP, int Port)
{
    g_SRr->AddServiceSource(g_GSS);
    return g_SRr->Start(IP, Port);
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
        if (!StartServiceRegistry(ServiceRegistryIP, ServiceRegistryPort))
        {
            std::cout << "StartServiceRegistry Error!\n";
            return -1;
        }
    }

    {
        if (!StartServiceDiscovery(ServiceDiscoveryIP, ServiceDiscoveryPort))
        {
            std::cout << "StartServiceDiscovery Error!\n";
            return -1;
        }
    }

    {
        if (!StartService(GameStatesServiceStubIP, GameStatesServiceStubPort))
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