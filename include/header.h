#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <thread>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <sys/time.h>
#include <sys/timerfd.h>
#include "../protobuf/myprotocol.pb.h"

#define LOGGERMODE_ON
#include "Log.h"

#define DEFAULT_TCP_PORT 2336
#define DEFAULT_UDP_PORT 2336

extern std::string LOCAL_IP;

struct Header
{
    int type = 0;
    int length = 0;
    Header(){};
    Header(int type) : type(type){};
};

void setnonblocking(int fd);

void addfd(int epollfd, int fd, bool block = true);

void delfd(int epollfd, int fd);

int get_new_socket(std::string IP, uint16_t socket_port, __socket_type protocol, sockaddr_in &sock_addr);

using namespace std;
template <typename T>
int Get_Header_Type(T &message)
{
    if (is_same<T, Message::UDP_INFO_REQ>::value)
    {
        return 800;
    }
    else if (is_same<T, Message::UDP_INFO_RES>::value)
    {
        return 801;
    }
    else if (is_same<T, Message::Hall_info_Response>::value)
    {
        return 201;
    }
    else if (is_same<T, Message::Hall_Message_Response>::value)
    {
        return 203;
    }
    else if (is_same<T, Message::Hall_EnterRoom_Response>::value)
    {
        return 205;
    }
    else if (is_same<T, Message::Room_info_Response>::value)
    {
        return 210;
    }
    else if (is_same<T, Message::Room_Message_Response>::value)
    {
        return 213;
    }
    else if (is_same<T, Message::Room_Set_tankid_Response>::value)
    {
        return 214;
    }
    else if (is_same<T, Message::Room_Start_Response>::value)
    {
        return 215;
    }
    else if (is_same<T, Message::Game_tankinfo_Response>::value)
    {
        return 220;
    }
    else if (is_same<T, Message::Game_bulletinfo_Response>::value)
    {
        return 221;
    }
    else if (is_same<T, Message::Game_brick_hited_Response>::value)
    {
        return 222;
    }
    else if (is_same<T, Message::Game_tank_hited_Response>::value)
    {
        return 223;
    }
    else if (is_same<T, Message::Game_destroyed_tank_Response>::value)
    {
        return 225;
    }
    return 0;
}

template <typename T>
void Send_TCP(int socket, T &message)
{
    Header header;
    header.type = Get_Header_Type(message);
    if (header.type == 0)
        return;
    header.length = message.ByteSizeLong();

    int len = sizeof(Header) + header.length;

    char *buf = new char[len];
    memset(buf, '\0', len);
    memcpy(buf, &header, sizeof(header));
    message.SerializeToArray(buf + sizeof(Header), header.length);

    send(socket, buf, len, 0);

    delete buf;
}

template <typename T>
void Send_UDP(int socket, sockaddr *dest_addr, T &message)
{
    Header header;
    header.type = Get_Header_Type(message);
    if (header.type == 0)
        return;
    header.length = message.ByteSizeLong();

    int len = sizeof(Header) + header.length;

    char *buf = new char[len];
    memset(buf, '\0', len);
    memcpy(buf, &header, sizeof(header));
    message.SerializeToArray(buf + sizeof(Header), header.length);

    send(socket, buf, len, 0);

    delete buf;
}