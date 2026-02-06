#include "NetworkMessageSender.h"

NetworkMessageSender::NetworkMessageSender(const std::string &gameId)
    : _gameId(gameId) {}

void NetworkMessageSender::SetGameService(std::shared_ptr<GameService> service)
{
    _service = service;
}

void NetworkMessageSender::SendToPlayer(const std::string &playerId, const json &message)
{
    _service->SendToPlayer(_gameId, playerId, message);
}

void NetworkMessageSender::Broadcast(const json &message,
                                     const std::vector<std::string> &exclude)
{
    _service->BroadcastToGame(_gameId, message, exclude);
}

void NetworkMessageSender::SendToPlayers(const std::vector<std::string> &playerIds,
                                         const json &message)
{
    _service->SendToPlayers(_gameId, playerIds, message);
}
