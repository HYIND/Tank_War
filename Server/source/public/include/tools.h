#pragma once

#include <stdint.h>
#include <iostream>
#include "Net/Helper/Buffer.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Tool
{
    int64_t GetTimestampMilliseconds();
    int64_t GetTimestampSecond();

	std::string GetFormatSecondStr(int64_t timestamp_seconds);

    std::string GenerateSimpleUuid();

    json ParseJson(const Buffer &buf);

    float RadianToAngle(float radian);
    float AngleToRadian(float angle);
}
