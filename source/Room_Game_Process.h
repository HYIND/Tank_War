#pragma once

#include "Tank_Server.h"
#include "Style.h"
#include "Map.h"

using namespace std;

extern int Get_Header_Type_bystring(string &str);

struct socket_messageinfo
{
    int socket;
    Header header;
    char *content = nullptr;
    socket_messageinfo(int socket) : socket(socket), content(nullptr) {}
    ~socket_messageinfo()
    {
        delete content;
    }
};

struct socket_sendinfo
{
    int socket;
    int len;
    char *send_ch;

    template <typename T>
    socket_sendinfo(int socket, T &message) : socket(socket)
    {
        Header header;
        header.type = Get_Header_Type(message);
        if (header.type == 0)
            return;
        header.length = message.ByteSizeLong();

        len = sizeof(Header) + header.length;
        send_ch = new char[len];
        memset(send_ch, '\0', len);

        memcpy(send_ch, &header, sizeof(header));
        message.SerializeToArray(send_ch + sizeof(Header), header.length);
    }

    socket_sendinfo(int socket, string &str) : socket(socket)
    {
        Header header;
        header.type = Get_Header_Type_bystring(str);
        if (header.type == 0)
            return;
        header.length = 0;
        len = sizeof(Header);
        send_ch = new char[len];
        memset(send_ch, '\0', len);
        memcpy(send_ch, &header, len);
    };

    socket_sendinfo(int socket, int type) : socket(socket)
    {
        Header header(type);
        header.length = 0;
        len = sizeof(Header);
        send_ch = new char[len];
        memset(send_ch, '\0', len);
        memcpy(send_ch, &header, len);
    }

    ~socket_sendinfo()
    {
        if (send_ch)
            delete []send_ch;
    }
};

struct sock_info;
struct Room_userinfo
{
    int tank_id;
    sock_info *sockinfo = NULL;
    bool Ready = false;
};

// 房间/游戏处理类
class Room_Process
{
public:
    int room_id = 0;                          //房间号
    int user_count = 0;                       //当前玩家人数
    int user_limited = 5;                     //玩家人数上限
    int socket_host = 0;                      //房主socket
    unordered_map<int, Room_userinfo *> info; //房内玩家信息 socket->userinfo

    int map_id = 0;                       //当前地图号
    unordered_map<int, Tank *> Tank_info; //房内玩家对应Tank信息  socket>tankinfo
    Map map_info;

    int recv_pipe[2];        //接收线程管道
    char buffer[100];        //读缓冲
    int recv_epoll;          //接收epoll
    epoll_event events[100]; // epoll events

    bool stop = false; //控制epoll循环

    queue<socket_messageinfo *> recv_queue; //消息接收队列
    queue<socket_sendinfo *> send_queue;    //消息发送队列

    // mutex
    mutex process_mtx;             //接收线程唤醒处理线程的锁
    condition_variable process_cv; //接收线程唤醒处理线程的条件变量

    mutex send_mtx;             //处理线程唤醒发送线程的锁
    condition_variable send_cv; //处理线程唤醒发送线程的条件变量

    mutex info_mtx;      //访问用户信息的锁
    mutex recvqueue_mtx; //访问接收队列的锁
    mutex sendqueue_mtx; //访问发送队列的锁

public:
    Room_Process(int socket); //构造函数，初始化；

    /*以下为房间消息的相关处理函数*/
    void Add_player(int socket);                                         //为房间添加一个新用户
    void Ready(int socket);                                              //准备
    void Cancel_Ready(int socket);                                       //取消准备
    string Start_Game(int socket);                                       //开始游戏
    string Quit_Room(int socket);                                        //退出房间
    void Return_Roominfo(int socket);                                    //返回房间信息
    void Room_Message(int socket, Header &header, char *content);        //发送消息
    void Set_Map(int id);                                                //设置地图信息
    void Change_Map(int socket, string &new_id_str);                     //房主切换地图
    string return_class_room(int socket, Header &header, char *content); //处理房间消息的入口函数，负责解析消息并调用相应的函数

    /*以下为游戏消息的相关处理函数*/
    void confim();    //开始游戏前最后同步地图ID，同时检查有无用户掉线
    void EndGame();   //检查游戏是否达到结束要求
    void Init_Game(); //初始化游戏

    void Refreash_tankinfo(int socket, Header &header, char *content);   //刷新坦克信息
    void Refreash_bulletinfo(int socket, Header &header, char *content); //刷新位置信息
    bool Hit_tank(int socket, Header &header, char *content);            //坦克击中校验
    bool Hit_brick(int socket, Header &header, char *content);           //墙体击中校验
    void hittank_notify(int hited_socket, int hited_id, int health);     // Tank受击消息通知
    void hitbrick_notify(int hited_brick_id, int health);                // Brick受击消息通知
    void destroy(int hited_socket, int hited_id);                        //摧毁消息通知

    int return_class_game(int socket, Header &header, char *content); //处理房间消息的入口函数，负责解析消息并调用相应的函数

    void run();          //初始运行函数，运行三个线程：接收线程、房间处理线程、发送线程
    void recv_process(); //接收线程，负责接收数据并加入消息队列
    void room_process(); //房间消息处理线程，负责处理消息队列中的数据
    void game_process(); //游戏消息处理线程，负责处理消息队列中的数据
    void send_process(); //发送线程，负责将发送队列中的消息发回

    void Refreash_Tank_process(bool *end);   //定时广播局内信息的线程
    void Refreash_Bullet_process(bool *end); //定时广播局内信息的线程

    ~Room_Process();
};

// class Room
// {
// public:
//     int host;
//     map<int, Room_userinfo *>;
//     room_info *roominfo;
//     int recv_pipe[2];
//     char buffer[1024];
//     int recv_epoll = epoll_create(50);
//     epoll_event events[100];
//     //分别检测两个用户是否掉线
//     bool stop1 = false;
//     bool stop2 = false;
//     bool stop = false; //控制epoll循环
//     queue<socket_messageinfo *> recv_queue;
//     queue<socket_messageinfo *> send_queue;
//     // mutex
//     mutex process_mtx;             //接收线程唤醒处理线程的锁
//     condition_variable process_cv; //接收线程唤醒处理线程的条件变量
//     mutex send_mtx;             //处理线程唤醒发送线程的锁
//     condition_variable send_cv; //处理线程唤醒发送线程的条件变量
//     mutex recvqueue_mtx; //接收队列的锁
//     mutex sendqueue_mtx; //发送队列的锁
// public:
//     //构造函数，初始化；
//     Game_Process(int socket1, int socket2, room_info *roominfo);
// };

// class Game_Process
// {
// public:
//     int room_id;

//     int socket1;
//     int socket2;

//     bool ready = false;

//     int recv_pipe[2];

//     char buffer[1024];
//     int recv_epoll = epoll_create(50);
//     epoll_event events[100];

//     bool stop = false; //控制epoll循环

//     queue<socket_messageinfo *> recv_queue;
//     queue<socket_messageinfo *> send_queue;

//     // mutex
//     mutex process_mtx;             //接收线程唤醒处理线程的锁
//     condition_variable process_cv; //接收线程唤醒处理线程的条件变量

//     mutex send_mtx;             //处理线程唤醒发送线程的锁
//     condition_variable send_cv; //处理线程唤醒发送线程的条件变量

//     mutex recvqueue_mtx; //接收队列的锁
//     mutex sendqueue_mtx; //发送队列的锁

// public:
// };