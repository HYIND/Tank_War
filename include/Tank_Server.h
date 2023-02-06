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
    int tcp_fd = -1;
    int udp_fd = -1;
    sockaddr_in tcp_addr;
    sockaddr_in udp_local_addr;
    sockaddr_in udp_remote_addr;
    string userid;
    int states = hall;
    bool udp_states = false;
    sock_info(int acc, sockaddr_in tcp_addr_in)
    {
        tcp_fd = acc;
        tcp_addr = tcp_addr_in;

        userid = "player" + to_string(acc);
    }

    void send_udp_info(int fd)
    {
        if (udp_fd == -1)
        {
            udp_fd = get_new_socket(LOCAL_IP, DEFAULT_UDP_PORT, SOCK_DGRAM, udp_local_addr);
        }

        Message::UDP_INFO_REQ Req;
        Req.set_ip("0.0.0.0");
        Req.set_port(ntohs(udp_local_addr.sin_port));

        Send_TCP(fd, Req);
    }

    void recv_udp_info(Header &header, char *content)
    {
        Message::UDP_INFO_REQ Req;
        Req.ParseFromArray(content, header.length);

        if (Req.ip() == "0.0.0.0")
            udp_remote_addr.sin_addr.s_addr = tcp_addr.sin_addr.s_addr;
        else
            udp_remote_addr.sin_addr.s_addr = inet_addr(Req.ip().c_str());
        udp_remote_addr.sin_port = htons(Req.port());

        if (connect(udp_fd, (sockaddr *)&udp_remote_addr, sizeof(struct sockaddr)) < 0)
            return;

        udp_states = true;
        Message::UDP_INFO_RES Res;
        Res.set_ok(true);

        Send_TCP(tcp_fd, Res);
    }
};

class Room_Process;

extern vector<sock_info *> user_list;
extern vector<Room_Process *> room_list;
extern vector<int> game_pipe_list;

extern int listen_epoll;
extern int hall_epoll;

extern epoll_event listen_events[100];
extern epoll_event hall_events[200];

extern int listen_pipe[2];
extern int hall_pipe[2];

extern int room_counter;

void sig_handler(int sig);

void addsig(int sig);

string get_IP(int socket);

string get_userid(int socket);

int Get_Header_Type_bystring(string &str);

void send_string(int socket, string s);

void server_listen(int listen_socket);