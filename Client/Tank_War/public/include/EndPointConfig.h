#pragma once

#include <iostream>

constexpr int offset = 5;

const std::string ServiceRegistryIP = "192.168.58.130";
constexpr int ServiceRegistryPort = 25059 + offset;

const std::string ServiceDiscoveryIP = "192.168.58.130";
constexpr int ServiceDiscoveryPort = 26070 + offset;

const std::string GameStatesServiceStubIP = "127.0.0.1";
constexpr int GameStatesServiceStubPort = 27080 + offset;

const std::string UserFrontendServiceIP = "192.168.58.130";
constexpr int UserFrontendServicePort = 28092;
const std::string UserFrontendServiceStubIP = "127.0.0.1";
constexpr int UserFrontendServiceStubPort = 28092 + offset;

const std::string GameServiceIP = "192.168.58.130";
constexpr int GameServicePort = 29100 + offset;
const std::string GameServiceStubIP = "127.0.0.1";
constexpr int GameServiceStubPort = 29100 + offset;
