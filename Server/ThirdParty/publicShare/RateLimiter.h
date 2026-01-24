#pragma once

#ifdef __linux__
#include <mutex>

using namespace std;

#define RETRY_IMEDIATELY_TIMES 30
#define RATE_LIMIT 50

class RateLimiter
{
public:
    RateLimiter(int qps);

    bool tryGetToken(); // 尝试获得令牌

    void pass();

private:
    long now();          // 获得当前时间, 单位ns
    void supplyTokens(); // 更新令牌桶中的令牌
    void mustGetToken(); // 必定成功获得令牌

    int bucketSize;        // 令牌桶大小
    int tokenLeft;         // 剩下的令牌数
    long supplyUnitTime;   // 补充令牌的单位时间
    long lastAddTokenTime; // 上次补充令牌的时间
    std::mutex mtx;        // 互斥锁
};

#endif