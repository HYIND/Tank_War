#include "Net/Core/NetCore.h"
#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "UserFrontendService.h"
#include "ServiceRegistrar.h"
#include "EndPointConfig.h"
#include "ServiceDiscoveryClient.h"
#include "stdafx.h"

void OnInterrupt()
{
	StopNetCoreLoop();
	exit(0);
}

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
		OnInterrupt();
		return TRUE;

	default:
		return FALSE;
	}
}
#elif __linux__
void signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		OnInterrupt();
	}
}
#endif

auto g_ServiceRegistrar = std::make_shared<ServiceRegistrar>();
auto g_userfrontendservice = std::make_shared<UserFrontendService>();
bool StartUserFrontendService(
	const std::string& IP, int Port,
	const std::string& stub_IP, int stub_Port,
	const std::string& gameStateService_IP, int gameStateService_Port)
{
	g_userfrontendservice->SetServiceEndPoint(IP, Port);
	g_userfrontendservice->SetStubEndPoint(stub_IP, stub_Port);
	g_userfrontendservice->SetGameStateEndPoint(gameStateService_IP, gameStateService_Port);
	return g_userfrontendservice->Start();
}

bool StartServiceRegistrar(const std::string& IP, int Port)
{
	g_ServiceRegistrar->AddServiceSource(g_userfrontendservice);
	return g_ServiceRegistrar->Start(IP, Port);
}

int main()
{
	system("chcp 65001 > nul"); // 切换到 UTF-8

	// LOGGER->SetLoggerPath("server.log");
	InitNetCore();
	RunNetCoreLoop();


#ifdef _WIN32
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		printf("Failed to set control handler\n");
		return 1;
	}
#elif __linux__
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
#endif

	{
		if (!StartUserFrontendService(UserFrontendServiceIP, UserFrontendServicePort,
			UserFrontendServiceStubIP, UserFrontendServiceStubPort,
			GameStatesServiceStubIP, GameStatesServiceStubPort))
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