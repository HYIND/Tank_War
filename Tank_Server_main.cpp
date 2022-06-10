#include "Tank_Server.h"

using namespace std;

vector<sock_info> hall_list;
vector<room_info> room_list;
vector<int> room_user;
vector<int> game_pipe_list;

unordered_map<int, int> two_user1;
unordered_map<int, int> two_user2;
unordered_map<int, Tank *> Tank_info;

int listen_epoll = epoll_create(100);
epoll_event listen_events[100];

int hall_epoll = epoll_create(100);
epoll_event hall_events[200];

int game_epoll = epoll_create(100);
epoll_event game_events[100];

int listen_pipe[2];
int con_pipe[2];
int game_pipe[2];

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
                for (auto it = hall_list.begin(); it != hall_list.end(); it++)
                {
                    if (it->accept == socket)
                    {
                        hall_list.erase(it);
                        break;
                    }
                }
                close(socket);
            }
            if (hall_events->events & EPOLLIN)
            {
                int re_num = recv(socket, buffer, 1023, 0);
                if (re_num > 0)
                {
                    string re = buffer;
                    string ret = return_class(socket, re);
                    if (ret != "NULL")
                        send(socket, (const char *)&(ret[0]), 1023, 0);
                }
                else if (re_num == 0)
                {
                    for (auto it = hall_list.begin(); it != hall_list.end(); it++)
                    {
                        if (it->accept == socket)
                        {
                            hall_list.erase(it);
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
                    hall_list.emplace_back(sock_info(socket, client));
                    addfd(hall_epoll, socket);
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

void server_game()
{
    char buffer[1024];
    memset(buffer, '\0', 1024);
    bool stop = false;
    while (!stop)
    {
        int num = epoll_wait(game_epoll, game_events, 100, -1);
        if (num < 0 && (errno != EINTR))
        {
            cout << "game_epoll failed!";
            break;
        }

        for (int i = 0; i < num; i++)
        {
            int socket = game_events[i].data.fd;
            if ((socket == game_pipe[0]) && (game_events->events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(game_pipe[0], signals, 1023, 0);
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
    }
}

int main()
{

    sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(2336);
    sock_addr.sin_addr.s_addr = inet_addr("10.0.20.4");

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

    thread T1(server_listen, mysocket);

    thread T2(server_hall);

    thread T3(server_game);
    // T1.detach();
    // T2.detach();
    // T3.detach();

    T3.join();
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
