#include "Tank_Server.h"
#include "Game_Process.h"

vector<sock_info> user_list;
vector<room_info> room_list;
vector<int> room_user;
vector<int> game_pipe_list;

unordered_map<int, int> two_user1;
unordered_map<int, int> two_user2;
unordered_map<int, Tank *> Tank_info;

int listen_epoll = epoll_create(100);
int hall_epoll = epoll_create(100);
int game_epoll = epoll_create(100);

epoll_event listen_events[100];
epoll_event hall_events[200];
epoll_event game_events[100];

int listen_pipe[2];
int con_pipe[2];
int game_pipe[2];

int room_counter = 1;

string get_IP(int socket)
{
    for (auto &v : user_list)
    {
        if (v.accept == socket)
            return inet_ntoa(v.addr.sin_addr);
    }
    return "";
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
    for (auto &v : user_list)
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
    for (auto &v : user_list)
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
    for (auto &v : user_list)
    {
        if (v.accept == sock_accept)
            v.states = room;
    }
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
                for (auto &v : user_list)
                {
                    if (v.accept == sock_accept)
                        v.states = room;
                }
                two_user1[v.user1] = sock_accept;
                two_user2[v.user2] = v.user1;
                string send_str = "EnterRoom:" + s;
                send(sock_accept, (const char *)&(send_str[0]), 1023, 0);
            }
            else
            {
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
        for (auto &v : user_list)
        {
            if (v.accept == user1)
                v.states = gaming;
            if (v.accept == user2)
                v.states = gaming;
        }
        room_info *roominfo = NULL;
        for (auto &v : room_list)
        {
            if (v.user1)
                roominfo = &v;
        }
        Game_Process *GP = new Game_Process(user1, user2, roominfo);
        GP->run();
        
        // thread T(game, user1, user2, roominfo);
        // T.detach();
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
            {
                for (auto &v : user_list)
                {
                    if (v.accept == it->user2)
                        v.states = hall;
                }
                two_user1[user] = 0;
                two_user2[it->user2] = 0;
                send(it->user2, (const char *)&str[0], 1023, 0);
            }
            for (auto &v : user_list)
            {
                if (v.accept == it->user1)
                    v.states = hall;
            }
            auto room_user_it = find(room_user.begin(), room_user.end(), it->user1);
            room_user.erase(room_user_it);
            room_list.erase(it);
            return;
        }
        else if (it->user2 == user)
        {
            it->user2 = 0;
            two_user1[it->user1] = 0;
            two_user2[user] = 0;
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
    if (temp == "ping")
    {
        send(sock_accept, (const char *)&(s[0]), 1023, 0);
    }
    else if (temp == "Getroom")
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
    else if (temp == "QuitRoom")
    {
        Quit_Room(sock_accept);
    }
    else if (temp == "StartGame")
    {
        Start_Game(sock_accept);
    }
    return "NULL";
}