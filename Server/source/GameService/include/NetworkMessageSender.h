#pragma once

#include "GameService.h"

class GameService;
class NetworkMessageSender
{
private:
    std::shared_ptr<GameService> _service;
    std::string _gameId;

public:
    NetworkMessageSender(const std::string &gameId);

    void SetGameService(std::shared_ptr<GameService> service);
    void SendToPlayer(const std::string &playerId, const json &message);
    void Broadcast(const json &message, const std::vector<std::string> &exclude = {});
    void SendToPlayers(const std::vector<std::string> &playerIds, const json &message);
};