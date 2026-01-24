#pragma once

namespace LoginSubServiceCommand
{

    constexpr int LoginSubService_Login = 1000;
    constexpr int LoginSubService_LoginRes = 2000;

    constexpr int LobbySubService_Logout = 1001;
    constexpr int LobbySubService_LogoutRes = 2001;
}

namespace ServiceDiscoveryCommand
{

    constexpr int ServiceDiscovery_GetAllServiceInfo = 30000;
    constexpr int ServiceDiscovery_GetAllServiceInfoRes = 31000;

    constexpr int ServiceDiscovery_GetServiceInfo = 30001;
    constexpr int ServiceDiscovery_GetServiceInfoRes = 31001;
}

namespace ServiceRegistryCommand
{
    constexpr int ServiceRegistry_UpdateServiceInfo = 80000;
    constexpr int ServiceRegistry_UpdateServiceInfoRes = 81000;
}
