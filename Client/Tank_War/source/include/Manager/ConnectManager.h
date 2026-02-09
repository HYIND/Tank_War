#pragma once

#include "ApplicationLayerCommunication/JsonProtocolClient.h"
#include "ServiceRegistryData.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

//用于管理与服务器的连接

enum class ConnectStatus
{
	disconnect = 0,
	connecting = 1,
	connected = 2
};

class ConnectManager
{

public:
	static ConnectManager* Instance();
	~ConnectManager();

	bool Login();
	void Logout();

	bool LoginGameSeervice(NetworkEndpoint endpoint);
	bool LogoutGameSeervice();

	bool SocialSend(json& js);
	bool SocialRequest(json& js_req, json& js_resp);

	bool GameSend(json& js_);
	bool GameRequest(json& js_req, json& js_resp);

	ConnectStatus connectStatus();

public:
	void OnSessionClose(JsonProtocolClient* _socaialSession);
	void OnRecvMessage(JsonProtocolClient* session, json& src);

	std::shared_ptr<JsonProtocolClient> SocialSession();
	std::shared_ptr<JsonProtocolClient> GameSession();

private:
	ConnectManager();

	bool InnerSocialSend(const json& js);
	bool InnerGameSend(const json& buf);

	bool InnerSocialRequest(const json& js_req, json& js_resp);
	bool InnerGameRequest(const json& js_req, json& js_resp);

	bool ProcessLoginRequest();
	bool ProcessLogoutRequest();

	bool ProcessLoginGameSeerviceRequest();
	bool ProcessLogoutGameSeerviceRequest();

private:
	ConnectStatus _status = ConnectStatus::disconnect;
	std::shared_ptr<JsonProtocolClient> _socaialSession = nullptr;
	std::shared_ptr<JsonProtocolClient> _gameSession = nullptr;
};

#define CONNECTMANAGER ConnectManager::Instance()
