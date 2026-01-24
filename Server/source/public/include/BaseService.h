#pragma once

#include "ServiceRegistryData.h"

using namespace ServiceRegistryDataDef;

class BaseService
{
public:
    virtual std::vector<ServiceInfo> GetServiceInfo() = 0;
};