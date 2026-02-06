#pragma once

#include "stdafx.h"
#include "Net/Helper/Buffer.h"

class RequestManager
{
public:
	static RequestManager* Instance();

public:
	void SendHallMsg(const std::string& msg);
	void SendRoomMsg(const std::string& msg);
	void SendPrivateMsg(const std::string& goaltoken, const std::string& msg);
	void RequestLobbyUserData();
	void RequestLobbyRoomData();

	void RequestRoomInfo();
	bool RequestCreateRoom();
	bool RequestJoinRoom(const std::string& roomid);
	bool RequestLeaveRoom();
	bool RequestChangeReadyStatus(bool isready);
	bool RequestStartGame(json& response);

private:
	RequestManager();
	~RequestManager();

};

#define REQUESTMANAGER RequestManager::Instance()