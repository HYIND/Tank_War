#pragma once

#include <iostream>
#include "Net/Session/BaseNetWorkSession.h"

struct LobbyUser
{
    std::string token;
    std::string name;
    BaseNetWorkSession *session;
};