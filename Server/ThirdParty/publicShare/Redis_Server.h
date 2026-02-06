#pragma once

#ifdef __linux__
#include <sw/redis++/redis++.h>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "fmt/core.h"
#include "Config.h"

#include "PublicShareExportMacro.h"

enum class RedisResult
{
    RedisOK = 0,           // 查询正常返回
    RedisNotFind = -1,     // 未查找到相应的信息
    RedisNotConnect = -2,  // 未连接
    RedisTimeOut = -3,     // 请求超时
    RedisReplyError = -4,  // 回复错误
    RedisClosedError = -5, // 对端关闭
    RedisIoError = -6,     // IO错误
    RedisOtherError = -7,  // 其他错误
};

class PUBLICSHARE_API Redis_Server
{
public:
    RedisResult Connect();
    RedisResult Connect(std::string &host, std::string &password, int port = 6379, int Database = 0);
    RedisResult Connect(std::string &&host, std::string &&password = "", int port = 6379, int Database = 0);
    RedisResult Set(std::string &key, std::string &value);
    RedisResult Set(std::string &key, const char *ch, size_t length);
    RedisResult Get(std::string &key, std::string &value_out);
    RedisResult Get(std::string &key, char *ch_out, size_t &length_out);
    RedisResult Get(std::vector<std::string> &keys, std::vector<sw::redis::OptionalString> &values_out);
    RedisResult Get(std::vector<std::string> &keys, std::unordered_map<std::string, std::string> &values_out);
    RedisResult PushList(std::string &key, std::vector<std::string> &list);
    RedisResult GetList(std::string &key, std::vector<std::string> &list_out);
    RedisResult SetHash(std::string &key, std::string &hash_key, std::string &hash_value);
    RedisResult SetHash(std::string &key, std::unordered_map<std::string, std::string> &hash_map);
    RedisResult GetHashMap(std::string &key, std::unordered_map<std::string, std::string> &hash_map);
    RedisResult GetHashMapByKeys(std::string &key, std::vector<std::string> &hash_keys, std::vector<sw::redis::OptionalString> &hash_values_out);
    RedisResult GetHashMapByKeys(std::string &key, std::vector<std::string> &hash_keys, std::unordered_map<std::string, std::string> &hash_map_out);
    RedisResult AddSet(std::string &key, std::string &value);
    RedisResult AddSet(std::string &key, std::unordered_set<std::string> &values);
    RedisResult GetSets(std::string &key, std::vector<std::string> &values_out);
    RedisResult GetSets(std::string &key, std::unordered_set<std::string> &values_out);
    RedisResult IsSetMemberExist(std::string &key, std::string &value);

private:
    sw::redis::Redis redis;
    bool connect = false;
};

#endif