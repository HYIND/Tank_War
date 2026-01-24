#include "GameService.h"
#include "ServiceRegistrar.h"
#include "EndPointConfig.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        StopNetCoreLoop();
    }
}

auto g_ServiceRegistrar = std::make_shared<ServiceRegistrar>();
auto g_gameservice = std::make_shared<GameService>();

bool StartService(const std::string &IP, int Port)
{
    auto conn = std::make_shared<JsonCommunicateConnectManager>();
    g_gameservice->SetConnectManager(conn);
    return g_gameservice->Start(IP, Port);
}

bool StartServiceRegistrar(const std::string &IP, int Port)
{
    g_ServiceRegistrar->AddServiceSource(g_gameservice);
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
        if (!StartService(GameServiceIP, GameServicePort))
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