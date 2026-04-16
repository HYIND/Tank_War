#pragma once

#include <iostream>
#include "Config.h"

const std::string ServiceRegistryListenIP = Config::Instance()->Read(std::string("ServiceRegistryListenIP"), std::string(""));
const int ServiceRegistryListenPort = Config::Instance()->Read(std::string("ServiceRegistryListenPort"), 0);

const std::string ServiceDiscoveryListenIP = Config::Instance()->Read(std::string("ServiceDiscoveryListenIP"), std::string(""));
const int ServiceDiscoveryListenPort = Config::Instance()->Read(std::string("ServiceDiscoveryListenPort"), 0);

const std::string GameStatesServiceStubIP = Config::Instance()->Read(std::string("GameStatesServiceStubIP"), std::string(""));
const int GameStatesServiceStubPort = Config::Instance()->Read(std::string("GameStatesServiceStubPort"), 0);

const std::string UserFrontendServiceIP = Config::Instance()->Read(std::string("UserFrontendServiceIP"), std::string(""));
const int UserFrontendServicePort = Config::Instance()->Read(std::string("UserFrontendServicePort"), 0);
const std::string UserFrontendServiceStubIP = Config::Instance()->Read(std::string("UserFrontendServiceStubIP"), std::string(""));
const int UserFrontendServiceStubPort = Config::Instance()->Read(std::string("UserFrontendServiceStubPort"), 0);

const std::string GameServiceIP = Config::Instance()->Read(std::string("GameServiceIP"), std::string(""));
const int GameServicePort = Config::Instance()->Read(std::string("GameServicePort"), 0);
const std::string GameServiceStubIP = Config::Instance()->Read(std::string("GameServiceStubIP"), std::string(""));
const int GameServiceStubPort = Config::Instance()->Read(std::string("GameServiceStubPort"), 0);


const std::string ServiceRegistryIP = Config::Instance()->Read(std::string("ServiceRegistryIP"), std::string(""));
const int ServiceRegistryPort = Config::Instance()->Read(std::string("ServiceRegistryPort"), 0);

const std::string ServiceDiscoveryIP = Config::Instance()->Read(std::string("ServiceDiscoveryIP"), std::string(""));
const int ServiceDiscoveryPort = Config::Instance()->Read(std::string("ServiceDiscoveryPort"), 0);

const std::string GameStatesServiceStubReportIP = Config::Instance()->Read(std::string("GameStatesServiceStubReportIP"), std::string(""));
const int GameStatesServiceStubReportPort = Config::Instance()->Read(std::string("GameStatesServiceStubReportPort"), 0);

const std::string UserFrontendServiceReportIP = Config::Instance()->Read(std::string("UserFrontendServiceReportIP"), std::string(""));
const int UserFrontendServiceReportPort = Config::Instance()->Read(std::string("UserFrontendServiceReportPort"), 0);
const std::string UserFrontendServiceStubReportIP = Config::Instance()->Read(std::string("UserFrontendServiceStubReportIP"), std::string(""));
const int UserFrontendServiceStubReportPort = Config::Instance()->Read(std::string("UserFrontendServiceStubReportPort"), 0);

const std::string GameServiceReportIP = Config::Instance()->Read(std::string("GameServiceReportIP"), std::string(""));
const int GameServiceReportPort = Config::Instance()->Read(std::string("GameServiceReportPort"), 0);
const std::string GameServiceStubReportIP = Config::Instance()->Read(std::string("GameServiceStubReportIP"), std::string(""));
const int GameServiceStubReportPort = Config::Instance()->Read(std::string("GameServiceStubReportPort"), 0);
