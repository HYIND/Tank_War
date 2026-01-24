#pragma once

#include <iostream>

std::string GnenerateJwtStr(const std::string &token);
bool VerfiyJwtToken(const std::string &jwtstr, std::string &token);