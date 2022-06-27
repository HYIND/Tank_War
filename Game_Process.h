#pragma once

#include "Tank_Server.h"

using namespace std;

struct socket_messageinfo
{
    int socket;
    char ch[1024];
    socket_messageinfo(int socket, char ch[])
    {
        this->socket = socket;
        memcpy(this->ch, ch, 1024);
    }
    // : socket(socket), ch(ch) {}
};

//游戏处理线程
class Game_Process
{
public:
    int socket1;
    int socket2;

    Tank *Tank1;
    Tank *Tank2;

    room_info *roominfo;

    int recv_pipe[2];

    char buffer[1024];

    int recv_epoll = epoll_create(50);
    epoll_event events[100];

    //分别检测两个用户是否掉线
    bool stop1 = false;
    bool stop2 = false;

    bool stop = false; //控制epoll循环

    queue<socket_messageinfo *> recv_queue;
    queue<socket_messageinfo *> send_queue;

    // mutex

    mutex process_mtx;             //接收线程唤醒处理线程的锁
    condition_variable process_cv; //接收线程唤醒处理线程的条件变量

    mutex send_mtx;             //处理线程唤醒发送线程的锁
    condition_variable send_cv; //处理线程唤醒发送线程的条件变量

public:
    //析构函数，初始化；
    Game_Process(int socket1, int socket2, room_info *roominfo);

    //运行函数，运行三个线程
    void run();

    //接收线程，负责接收数据并加入消息队列
    void recv_process();

    //处理线程，负责处理消息队列中的数据
    void game_process();

    //发送线程，负责将发送队列中的消息发回
    void send_process();
};
