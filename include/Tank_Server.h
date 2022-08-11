#pragma once

#include "header.h"
#include "Room_Game_Process.h"

using namespace std;

enum
{
    hall,
    room,
    gaming,
    disconnection
};

struct sock_info
{
    int accept;
    sockaddr_in addr;
    string userid;
    int states = hall;
    sock_info(int acc, sockaddr_in add)
    {
        accept = acc;
        addr = add;
        userid = "player" + to_string(acc);
    }
};

class Room_Process;

extern vector<sock_info *> user_list;
extern vector<Room_Process *> room_list;
extern vector<int> room_user;
extern vector<int> game_pipe_list;

extern unordered_map<int, int> two_user1;
extern unordered_map<int, int> two_user2;

extern int listen_epoll;
extern int hall_epoll;
extern int game_epoll;

extern epoll_event listen_events[100];
extern epoll_event hall_events[200];
extern epoll_event game_events[100];

extern int listen_pipe[2];
extern int con_pipe[2];
extern int game_pipe[2];

int Get_Header_Type_bystring(string &str);

void send_string(int socket, string s);

void sig_handler(int sig);

void addsig(int sig);

string get_IP(int socket);

void Hall_Message(int sock_accept, string &send_str);

void Get_hall_info(int sock_accept);

void Create_Room(int sock_accept);

void Enter_Room(int sock_accept, string s);

void Quit_Room(int &user);

void Start_Game(int &user1);

void return_class(int &sock_accept, Header &header, char *content);

void Tank_destroy(int scoket);

template <typename T>
int Get_Header_Type(T &message)
{
    if (is_same<T, Message::Hall_info_Response>::value)
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
void Send(int socket, T &message)
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
}