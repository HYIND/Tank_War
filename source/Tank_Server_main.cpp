#include "header.h"
#include "Tank_Server.h"
#include "Map.h"
#include <net/if.h>
#include <sys/ioctl.h>
using namespace std;

extern vector<sock_info *> user_list;
extern vector<Room_Process *> room_list;
extern vector<int> room_user;
extern vector<int> game_pipe_list;

extern unordered_map<int, int> two_user1;
extern unordered_map<int, int> two_user2;
extern unordered_map<int, Tank *> Tank_info;

extern int listen_epoll;
extern int hall_epoll;
extern int game_epoll;

extern epoll_event listen_events[100];
extern epoll_event hall_events[200];
extern epoll_event game_events[100];

extern int listen_pipe[2];
extern int con_pipe[2];
extern int game_pipe[2];

unordered_map<int, int> Room;

void server_hall()
{
    char buffer[1024];
    memset(buffer, '\0', 1024);
    bool stop = false;
    while (!stop)
    {
        int num = epoll_wait(hall_epoll, hall_events, 200, -1);
        if (num < 0 && (errno != EINTR))
        {
            cout << "hall_epoll failed!";
            break;
        }

        for (int i = 0; i < num; i++)
        {
            int socket = hall_events[i].data.fd;
            if ((socket == con_pipe[0]) && (hall_events->events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(con_pipe[0], signals, 1023, 0);
                if (ret == -1 || ret == 0)
                    continue;
                else
                {
                    for (i = 0; i < ret; i++)
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
            if (hall_events->events & EPOLLRDHUP)
            {
                delfd(hall_epoll, socket);
                for (auto it = user_list.begin(); it != user_list.end(); it++)
                {
                    if ((*it)->accept == socket)
                    {
                        if ((*it)->states == room)
                        {
                            // Quit_Room(socket);
                        }
                        user_list.erase(it);
                        break;
                    }
                }
                close(socket);
            }
            if (hall_events->events & EPOLLIN)
            {
                int re_num = recv(socket, buffer, 1023, 0);
                while (re_num > 0)
                {
                    string re = buffer;
                    string ret = return_class(socket, re);
                    if (ret != "NULL")
                        send(socket, (const char *)&(ret[0]), 1023, 0);
                    re_num = recv(socket, buffer, 1023, 0);
                }
                if (re_num == 0)
                {
                    for (auto it = user_list.begin(); it != user_list.end(); it++)
                    {
                        if ((*it)->accept == socket)
                        {
                            user_list.erase(it);
                            break;
                        }
                    }
                    close(socket);
                }
            }
        }
    }
}

void server_listen(int mysocket)
{
    addfd(listen_epoll, mysocket);
    int ret = listen(mysocket, 10);
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
                        for (i = 0; i < ret; i++)
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
                int socket = accept(mysocket, (struct sockaddr *)&client, &length);
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
                    // getsockopt(mysocket, SOL_SOCKET, SO_RCVBUF, (char *)&i, &j);

                    // thread T(server_hall, socket);
                    // T.detach();
                }
                else
                    cout << "socket accept fail!\n";
            }
        }
    }
    close(mysocket);
}

int get_local_ip(const char *eth_inf, char *out)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    strcpy(out, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(sd);
    return 0;
}

int main(int argc, char *argv[])
{
    sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(2336);

    if (argc > 1)
    {
        sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    // else
    // {
    //     char IP[20] = {'\0'};
    //     get_local_ip("eth0", IP);
    //     sock_addr.sin_addr.s_addr = inet_addr(IP);
    // }

    int mysocket = socket(PF_INET, SOCK_STREAM, 0);

    int result = 0;
    result = bind(mysocket, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr));
    if (result)
    {
        perror("bind socket error");
        return 0;
    }

    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, listen_pipe) != -1);
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, con_pipe) != -1);
    setnonblocking(listen_pipe[1]);
    addfd(listen_epoll, listen_pipe[0]);
    setnonblocking(con_pipe[1]);
    addfd(hall_epoll, con_pipe[0]);
    setnonblocking(game_pipe[1]);
    addfd(game_epoll, game_pipe[0]);
    addsig(SIGINT);
    addsig(SIGTERM);

    // int SendBuf = 1024 * 1024;
    // int RecvBuf = 1024 * 1024;
    // getsockopt(socket, SOL_SOCKET, SO_SNDBUF, &i, &j);
    // setsockopt(mysocket, SOL_SOCKET, SO_SNDBUF, (const char *)&SendBuf, sizeof(int));
    // setsockopt(mysocket, SOL_SOCKET, SO_RCVBUF, (const char *)&RecvBuf, sizeof(int));

    Init_Style();
    Init_Map();

    thread T1(server_listen, mysocket);

    thread T2(server_hall);

    // thread T3(server_game);
    // T1.detach();
    // T2.detach();
    // T3.detach();

    // T3.join();
    T2.join();
    T1.join();
    close(listen_pipe[0]);
    close(listen_pipe[1]);
    close(con_pipe[0]);
    close(con_pipe[1]);
    close(game_pipe[0]);
    close(game_pipe[1]);
    return 0;
}
