#pragma once

#include "ApplicationLayerCommunication/JsonProtocolSession.h"
#include "stdafx.h"

class GameSession
{
private:
    std::string _sessionId;
    std::string _playerId;
    std::string _gameId;

    JsonProtocolSession _session;

public:
    // 构造和访问方法
    GameSession(JsonProtocolSession session,
                const std::string &sessionId,
                const std::string &playerId,
                const std::string &gameId = "")
        : _session(session), _sessionId(sessionId), _playerId(playerId), _gameId(gameId) {}

    const std::string &getSessionId() const { return _sessionId; }
    const std::string &getPlayerId() const { return _playerId; }
    const std::string &getGameId() const { return _gameId; }

    const JsonProtocolSession &getSession() const { return _session; }

    void setGameId(const std::string &gameId) { _gameId = gameId; }

    bool operator==(const GameSession &other) const
    {
        return _sessionId == other._sessionId && _playerId == other._playerId && _gameId == other._gameId;
    }
};