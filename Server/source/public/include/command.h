#pragma once

namespace LoginSubServiceCommand
{

    constexpr int LoginSubService_Login = 1000;
    constexpr int LoginSubService_LoginRes = 2000;

}

namespace LobbySubServiceCommand
{
    constexpr int LobbySubService_Logout = 1001;
    constexpr int LobbySubService_LogoutRes = 2001;

    constexpr int LobbySubService_SendHallMsg = 1012;
    constexpr int LobbySubService_SendHallMsgRes = 2012;

    constexpr int LobbySubService_SendRoomMsg = 1013;
    constexpr int LobbySubService_SendRoomMsgRes = 2013;

    constexpr int LobbySubService_SendPrivateMsg = 1014;
    constexpr int LobbySubService_SendPrivateMsgRes = 2014;

    constexpr int LobbySubService_RequestLobbyUser = 1015;
    constexpr int LobbySubService_RequestLobbyUserRes = 2015;

    constexpr int LobbySubService_RequestLobbyRoom = 1016;
    constexpr int LobbySubService_RequestLobbyRoomRes = 2016;

    constexpr int LobbySubService_HallMsgBroadcast = 3001;
    constexpr int LobbySubService_RoomMsgBroadcast = 3002;
    constexpr int LobbySubService_PrivateMsgForward = 3003;

    constexpr int LobbySubService_RequestCreateRoom = 1017;
    constexpr int LobbySubService_RequestCreateRoomRes = 2017;

    constexpr int LobbySubService_RequestJoinRoom = 1018;
    constexpr int LobbySubService_RequestJoinRoomRes = 2018;

    constexpr int LobbySubService_RequestLeaveRoom = 1019;
    constexpr int LobbySubService_RequestLeaveRoomRes = 2019;

    constexpr int LobbySubService_RequestChangeReadyStatus = 1020;
    constexpr int LobbySubService_RequestChangeReadyStatusRes = 2020;

    constexpr int LobbySubService_RequestRoomInfo = 1021;
    constexpr int LobbySubService_RequestRoomInfoRes = 2021;

    constexpr int LobbySubService_RequestStartGame = 1022;
    constexpr int LobbySubService_RequestStartGameRes = 2022;
}

namespace ServiceDiscoveryCommand
{

    constexpr int ServiceDiscovery_GetAllServiceInfo = 30000;
    constexpr int ServiceDiscovery_GetAllServiceInfoRes = 31000;

    constexpr int ServiceDiscovery_GetServiceInfo = 30001;
    constexpr int ServiceDiscovery_GetServiceInfoRes = 31001;

    constexpr int ServiceDiscovery_GetServiceInfoByServiceIds = 30002;
    constexpr int ServiceDiscovery_GetServiceInfoByServiceIdsRes = 31002;
}

namespace ServiceRegistryCommand
{
    constexpr int ServiceRegistry_UpdateServiceInfo = 80000;
    constexpr int ServiceRegistry_UpdateServiceInfoRes = 81000;
}

namespace MessageDelegateCommand
{
    constexpr int MessageDelegate_UserMessageDelegate = 90000;
    constexpr int MessageDelegate_UserMessageDelegateRes = 91000;
}

namespace GameStateServiceCommand
{
    constexpr int GameStateService_FindUserEndPoint = 100000;
    constexpr int GameStateService_FindUserEndPointRes = 101000;

    constexpr int GameStateService_UserLogin = 100001;
    constexpr int GameStateService_UserLoginRes = 101001;

    constexpr int GameStateService_UserLogoutService = 100002;
    constexpr int GameStateService_UserLogoutServiceRes = 101002;

    constexpr int GameStateService_GetOnlineLobbyUser = 100003;
    constexpr int GameStateService_GetOnlineLobbyUserRes = 101003;

    constexpr int GameStateService_GetUserState = 100004;
    constexpr int GameStateService_GetUserStateRes = 101004;

    constexpr int GameStateService_GetAllLobbyRoom = 100005;
    constexpr int GameStateService_GetAllLobbyRoomRes = 101005;

    constexpr int GameStateService_CreateRoom = 100006;
    constexpr int GameStateService_CreateRoomRes = 101006;

    constexpr int GameStateService_JoinRoom = 100007;
    constexpr int GameStateService_JoinRoomRes = 101007;

    constexpr int GameStateService_LeaveRoom = 100008;
    constexpr int GameStateService_LeaveRoomRes = 101008;

    constexpr int GameStateService_ChangeReadyStatus = 100009;
    constexpr int GameStateService_ChangeReadyStatusRes = 101009;

    constexpr int GameStateService_RoomInfo = 100010;
    constexpr int GameStateService_RoomInfoRes = 101010;

    constexpr int GameStateService_StartGame = 100011;
    constexpr int GameStateService_StartGameRes = 101011;

    constexpr int GameStateService_PlayerLeaveGame = 100012;
    constexpr int GameStateService_PlayerLeaveGameRes = 101012;

    constexpr int GameStateService_GameEnd = 100013;
    constexpr int GameStateService_GameEndRes = 101013;

}

namespace GameServiceCommand
{
    constexpr int GameService_NewGame = 200000;
    constexpr int GameService_NewGameRes = 201000;

    constexpr int GameService_JoinGame = 200001;
    constexpr int GameService_JoinGameRes = 201001;

    constexpr int GameService_LeaveGame = 200002;
    constexpr int GameService_LeaveGameRes = 201002;

    constexpr int GameService_InputToGame = 200003;
    constexpr int GameService_InputToGameRes = 201003;

    constexpr int GameService_BroadCastGameState = 202000;

    constexpr int GameService_EliminateInfo = 202001;

    constexpr int GameService_GameOverInfo = 202002;
}