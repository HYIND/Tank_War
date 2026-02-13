#pragma once

#include "LobbySubService.h"
#include "ApplicationLayerCommunication/JsonProtocolServer.h"
#include "ServiceRegistryData.h"
#include "stdafx.h"

using namespace ServiceRegistryDataDef;

class LobbySubService;
class LoginSubService
{
public:
    LoginSubService();
    ~LoginSubService();

public:
    void OnSessionEstablish(JsonProtocolSession session);
    void OnRecvMessage(JsonProtocolSession session, json &src);
    void OnRecvRequest(JsonProtocolSession session, json &src, json &dest);
    void OnSessionClose(JsonProtocolSession session);

    void SetLobbySubService(std::shared_ptr<LobbySubService> service);
    void SetServer(std::shared_ptr<JsonProtocolServer> m);
    void SetGameStateStub(std::shared_ptr<JsonProtocolClient> stub);
    void SetServiceInfo(std::shared_ptr<ServiceInfo> info);

public:
    bool ConnectionEnter(JsonProtocolSession session);

private:
    void ProcessMsg(JsonProtocolSession session, json &src, std::vector<json> &dest);
    void ProcessLogin(JsonProtocolSession session, json &js_src, json &js_dest);

private:
    std::shared_ptr<JsonProtocolClient> _gameStateStub;

    std::shared_ptr<JsonProtocolServer> _server;
    std::weak_ptr<LobbySubService> _weaklobbyservice;

    std::shared_ptr<ServiceInfo> _serviceinfo;
};