#include "Tank_Server.h"

vector<sock_info *> user_list;
vector<Room_Process *> room_list;
vector<int> game_pipe_list;

int listen_epoll = epoll_create(100);
int hall_epoll = epoll_create(100);

epoll_event listen_events[100];
epoll_event hall_events[200];

int listen_pipe[2];
int hall_pipe[2];

int room_counter = 0;

string get_IP(int socket)
{
    for (auto &v : user_list)
    {
        if (v->accept == socket)
            return inet_ntoa(v->addr.sin_addr);
    }
    return "";
}

string get_userid(int socket)
{
    for (auto &v : user_list)
    {
        if (v->accept == socket)
            return v->userid;
    }
    return "";
}

void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(listen_pipe[1], (char *)&msg, 1, 0);
    send(hall_pipe[1], (char *)&msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

// int Get_Header_Type_bystring(string &str)
// {
//     return 0;
// }

// void send_string(int socket, string s)
// {
//     Header header;
//     header.type = Get_Header_Type_bystring(s);
//     if (header.type == 0)
//         return;
//     header.length = 0;
//     char send_buf[sizeof(Header)] = {'\0'};
//     memcpy(send_buf, &header, sizeof(Header));
//     send(socket, send_buf, sizeof(Header), 0);
// }

void server_listen(int listen_socket)
{
    addfd(listen_epoll, listen_socket);
    int ret = listen(listen_socket, 10);
    if (ret < 0 && errno != EINTR)
    {
        perror("listen socket error");
        return;
    }
    int stop = false;
    while (!stop)
    {
        int number = epoll_wait(listen_epoll, listen_events, 200, -1);
        if (number < 0)
        {
            cout << "listen_epoll failure\n";
            break;
        }
        for (int i = 0; i < number; i++)
        {
            if ((listen_events[i].data.fd == listen_pipe[0]) && (listen_events->events & EPOLLIN))
            {
                {
                    int sig;
                    char signals[1024];
                    int ret = recv(listen_pipe[0], signals, 1023, 0);
                    if (ret == -1 || ret == 0)
                        continue;
                    else
                    {
                        for (int i = 0; i < ret; i++)
                        {
                            switch (signals[i])
                            {
                            case SIGINT:
                            case SIGTERM:
                            {
                                stop = true;
                            }
                            }
                        }
                    }
                }
            }
            if (listen_events[i].events & EPOLLIN)
            {
                sockaddr_in client;
                socklen_t length = sizeof(client);
                int socket = accept(listen_socket, (struct sockaddr *)&client, &length);
                if (socket != -1)
                {
                    setnonblocking(socket);
                    sock_info *info = new sock_info(socket, client);
                    user_list.emplace_back(info);
                    int flag = 1;
                    setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));

                    addfd(hall_epoll, socket);

                    // int i = 0;
                    // socklen_t j = sizeof(i);
                    // getsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (char *)&i, &j);

                    // thread T(server_hall, socket);
                    // T.detach();
                }
                else
                    cout << "socket accept fail!\n";
            }
        }
    }
    close(listen_socket);
}

