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

void sig_handler(int sig);

void addsig(int sig);

string get_IP(int socket);

void Hall_Message(int sock_accept, string &send_str);

void Get_hall_user(int sock_accept, string &s);

void Get_hall_room(int sock_accept, string &s);

void Create_Room(int sock_accept);

void Enter_Room(int sock_accept, string s);

void Quit_Room(int &user);

void Start_Game(int &user1);

string return_class(int &sock_accept, string &s);

void Tank_destroy(int scoket);