#pragma once
#ifdef __linux__
#include <string>
#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "PublicShareExportMacro.h"

using namespace std;

class PUBLICSHARE_API MQ
{
public:
    MQ();
    MQ(string &hostname, int port, string &exchange, string &routingkey);
    virtual ~MQ(){};

    bool LoadConfig();

protected:
    string m_honstname;                 // 虚拟主机ip
    int m_port;                         // 端口号
    string m_exchange;                  // 交换器名称
    string m_routingkey;                // 绑定路由的key对应消费者
    amqp_connection_state_t connection; // 连接
    int reconnect;                      // 重新链接
};

// 消费者
class Consumer_MQ : public MQ
{
public:
    Consumer_MQ();
    Consumer_MQ(string &hostname, int port, string &exchange, string &routingkey);
    virtual ~Consumer_MQ(){};

public:
    bool Consumer_Connect();
    bool Consumer_BuildQueue();
    int Consumer(char *&msg, size_t &length);
    int Consumer_Close();
};

// 生产者
class Producer_MQ : public MQ
{
public:
    Producer_MQ();
    Producer_MQ(string &hostname, int port, string &exchange, string &routingkey);
    virtual ~Producer_MQ(){};

public:
    bool Producer_Connect();
    int Producer_Publish(char *msg, size_t length);
    int Producer_Close();
};

#endif