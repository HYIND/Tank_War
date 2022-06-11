#include "Tank_Server.h"
using namespace std;

extern vector<sock_info> hall_list;
extern vector<room_info> room_list;
extern vector<int> room_user;
extern vector<int> game_pipe_list;

extern unordered_map<int, int> two_user1;
extern unordered_map<int, int> two_user2;
extern unordered_map<int, Tank *> Tank_info;

extern int hall_epoll;
extern epoll_event hall_events[200];

extern int game_epoll;
extern epoll_event game_events[200];

extern int listen_pipe[2];
extern int con_pipe[2];
extern int game_pipe[2];

int room_counter = 1;

void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void delfd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

string get_IP(int socket)
{
    for (auto &v : hall_list)
    {
        if (v.accept == socket)
            return inet_ntoa(v.addr.sin_addr);
    }
}

void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(listen_pipe[1], (char *)&msg, 1, 0);
    send(con_pipe[1], (char *)&msg, 1, 0);
    send(game_pipe[1], (char *)&msg, 1, 0);
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

void Send_Message(int sock_accept, string &send_str)
{
    send_str = "HallMessage:" + get_IP(sock_accept) + "_content:" + send_str;
    for (auto &v : hall_list)
    {
        if (v.states == hall)
        {
            int send_sock = v.accept;
            if (send_sock == sock_accept)
                continue;
            try
            {
                send(send_sock, (const char *)&(send_str[0]), 1023, 0);
            }
            catch (exception &e)
            {
                continue;
            }
        }
    }
}

// 3user:172.11.33.55,123.44.33.22,888.9.3.4;
string Get_hall_user(int sock_accept, string &s)
{
    string re = "user:";
    for (auto &v : hall_list)
    {
        if (v.accept == sock_accept)
            continue;
        re += inet_ntoa(v.addr.sin_addr);
        re += ";";
    }
    return re;
}
void Get_hall_room(int sock_accept, string &s)
{
    string re_id = "roomid:";
    for (auto &v : room_list)
    {
        re_id += to_string(v.room_id);
        re_id += ";";
    }
    send(sock_accept, (const char *)&(re_id[0]), 1023, 0);
    string re = "room:";
    for (auto &v : room_user)
    {
        if (v == sock_accept)
            continue;
        re += get_IP(v);
        re += ";";
    }
    send(sock_accept, (const char *)&(re[0]), 1023, 0);
}

void Create_Room(int sock_accept)
{
    room_info newroom;
    newroom.room_id = room_counter;
    newroom.user1 = sock_accept;
    newroom.user2 = 0;
    room_list.emplace_back(newroom);
    room_user.emplace_back(sock_accept);
}

void Enter_Room(int sock_accept, string s)
{
    int id = atoi(s.c_str());
    for (auto &v : room_list)
    {
        if (v.room_id == id)
        {
            if (v.user2 == 0)
            {
                v.user2 = sock_accept;
                string send_str = "EnterRoom:" + s;
                send(sock_accept, (const char *)&(send_str[0]), 1023, 0);
                two_user1[v.user1] = sock_accept;
                two_user2[v.user2] = v.user1;
            }
        }
    }
}

void Start_Game(int &user1)
{
    int user2 = two_user1[user1];
    if (user2 != 0)
    {
        string send_str = "Start";
        send(user1, (const char *)&(send_str[0]), 1023, 0);
        send(user2, (const char *)&(send_str[0]), 1023, 0);
        delfd(hall_epoll, user1);
        delfd(hall_epoll, user2);
        for (auto &v : hall_list)
        {
            if (v.accept == user1)
                v.states = gaming;
            if (v.accept == user2)
                v.states = gaming;
        }
        thread T(game, user1, user2);
        T.detach();
    }
}

void Quit_Room(int &user)
{
    for (auto it = room_list.begin(); it != room_list.end(); it++)
    {
        if (it->user1 == user)
        {
            string str = "QuitRoom";
            if (it->user2 != 0)
                send(it->user2, (const char *)&str[0], 1023, 0);
            auto room_user_it = find(room_user.begin(), room_user.end(), it->user1);
            room_user.erase(room_user_it);
            room_list.erase(it);
            return;
        }
        else if (it->user2 == user)
        {
            it->user2 = 0;
            return;
        }
    }
}

string return_class(int &sock_accept, string &s)
{
    string::const_iterator iterStart = s.begin();
    string::const_iterator iterEnd = s.end();
    smatch m;
    regex reg("^[A-Z|a-z]+");
    regex_search(iterStart, iterEnd, m, reg);
    string temp;
    temp = m[0];
    if (temp == "Getroom")
        Get_hall_room(sock_accept, s);
    else if (temp == "Getuser")
        return Get_hall_user(sock_accept, s);
    else if (temp == "HallSend")
    {
        string send_str(m[0].second + 1, iterEnd);
        Send_Message(sock_accept, send_str);
    }
    else if (s == "CreateRoom")
    {
        Create_Room(sock_accept);
    }
    else if (temp == "EnterRoom")
    {
        string send_str(m[0].second + 1, iterEnd);
        Enter_Room(sock_accept, send_str);
    }
    else if (temp == "StartGame")
    {
        Start_Game(sock_accept);
    }
    else if (temp == "QuitRoom")
    {
        Quit_Room(sock_accept);
    }
    return "NULL";
}

void game_process(int &socket1, int &socket2, Tank &tank1, Tank &tank2, string &s)
{
    string::const_iterator iterStart = s.begin();
    string::const_iterator iterEnd = s.end();
    smatch m;
    regex reg("^[A-Z|a-z]+");
    regex_search(iterStart, iterEnd, m, reg);
    string temp;
    temp = m[0];
    if (temp == "mylocation")
    {
    }
}

void game(int socket1, int socket2)
{
    Tank *Tank1 = new Tank();
    Tank *Tank2 = new Tank();
    Tank_info[socket1] = Tank1;
    Tank_info[socket2] = Tank2;

    bool isalive = true;
    int pipe[2];
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, pipe) != -1);
    setnonblocking(pipe[1]);
    game_pipe_list.emplace_back(pipe[1]);

    int epoll = epoll_create(5);
    epoll_event events[100];

    addfd(epoll, pipe[0]);
    addfd(epoll, socket1);
    addfd(epoll, socket2);

    char buffer[1024];
    memset(buffer, '\0', 1024);

    bool stop = false;
    while (!stop)
    {
        int num = epoll_wait(epoll, events, 100, -1);
        if (num < 0 && (errno != EINTR))
        {
            cout << "epoll for game failed!";
            break;
        }
        for (int i = 0; i < num; i++)
        {
            int socket = events[i].data.fd;
            if ((socket == pipe[0]) && (events->events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(pipe[0], signals, 1023, 0);
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
            // if (events->events & EPOLLRDHUP)
            // {
            //     for (auto it = hall_list.begin(); it != hall_list.end(); it++)
            //     {
            //         if (it->accept == socket)
            //         {
            //             hall_list.erase(it);
            //             break;
            //         }
            //     }
            //     close(socket);
            // }
            if (events->events & EPOLLIN)
            {
                int re_num = recv(socket, buffer, 1023, 0);
                if (re_num > 0)
                {
                    int mysocket = socket1;
                    int opsocket = socket2;
                    if (socket != socket1)
                        swap(mysocket, opsocket);
                    return_game_class(mysocket, opsocket, buffer);
                }
            }
            // else if (re_num == 0)
            // {
            //     for (auto it = hall_list.begin(); it != hall_list.end(); it++)
            //     {
            //         if (it->accept == socket)
            //         {
            //             hall_list.erase(it);
            //             break;
            //         }
            //     }
            //     close(socket);
            // }
        }
    }
    free(Tank1);
    free(Tank2);
}

void return_game_class(int mysocket, int opsocket, char buf[])
{
    string re = buf;
    string::const_iterator iterStart = re.begin();
    string::const_iterator iterEnd = re.end();
    smatch m;
    regex reg("^[A-Z|a-z]+");
    regex_search(iterStart, iterEnd, m, reg);
    string temp = m[0];
    if (temp == "mylocation")
    {
        try
        {
            Tank *mytank = Tank_info[mysocket];
            memcpy(mytank, &buf[11], 24);
            char send_ch[1024] = "oplocation:";
            memcpy(&send_ch[11], &buf[11], 24);
            send(opsocket, (const char *)&(send_ch[0]), 1023, 0);
            memset(buf, '\0', 1024);
        }
        catch (exception &e)
        {
            return;
        }
    }
    else if (temp == "mybullet")
    {
        char send_ch[1024] = "opbullet:";
        memcpy(&send_ch[9], &buf[9], 1012);
        send(opsocket, (const char *)&(send_ch[0]), 1024, 0);
    }
}