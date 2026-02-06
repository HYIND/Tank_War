#pragma once

#include <iostream>
#include "ApplicationLayerCommunication/JsonProtocolSession.h"
#include "GameStateDef/RoomDef.h"

struct LobbyUser
{
    std::string token;
    std::string name;
    JsonProtocolSession session;
};
