#include "Tank_Server.h"

vector<sock_info *> user_list;
vector<Room_Process *> room_list;
vector<int> room_user;
vector<int> game_pipe_list;

unordered_map<int, int> two_user1;
unordered_map<int, int> two_user2;

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

void Hall_Message(int sock_accept, string &send_str)
{
    send_str = "HallMessage:" + get_userid(sock_accept) + "_content:" + send_str;
    for (auto &v : user_list)
    {
        if (v->states == hall)
        {
            int send_sock = v->accept;
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

// 3user:aaa,bbb,ccc;
void Get_hall_user(int sock_accept, string &s)
{
    string re = "user:";
    for (auto &v : user_list)
    {
        if (v->accept == sock_accept)
            continue;
        re += "#";
        re += v->userid;
        re += ";";
    }
    send(sock_accept, (const char *)&(re[0]), 1023, 0);
}
void Get_hall_room(int sock_accept, string &s)
{
    string re_id = "roomid:";
    string re = "room:";
    for (auto &v : room_list)
    {
        re_id += to_string(v->room_id);
        re_id += ";";

        re += "#";
        re += get_userid(v->socket_host);
        re += ";";
    }
    send(sock_accept, (const char *)&(re_id[0]), 1023, 0);
    // for (auto &v : room_user)
    // {
    //     if (v == sock_accept)
    //         continue;
    //     re += "#";
    //     re += get_userid(v);
    //     re += ";";
    // }
    send(sock_accept, (const char *)&(re[0]), 1023, 0);
}

void Create_Room(int sock_accept)
{
    Room_Process *newroom = new Room_Process(sock_accept);
    // room_info newroom;
    newroom->room_id = room_counter;
    newroom->socket_host = sock_accept;
    newroom->Add_player(sock_accept);
    room_list.emplace_back(newroom);
    room_user.emplace_back(sock_accept);

    thread T(&Room_Process::run, newroom);
    T.detach();
}

void Enter_Room(int sock_accept, string s)
{
    int id = atoi(s.c_str());
    for (auto &v : room_list)
    {
        if (v->room_id == id)
        {
            if (v->user_count < v->user_limited)
            {
                string send_str = "EnterRoom:" + s;
                send(sock_accept, (const char *)&(send_str[0]), 1023, 0);
                v->Add_player(sock_accept);
            }
            //房间人数已满
            else
            {
            }
        }
        //房间不存在/已解散
        else
        {
        }
    }
}

void setuserid(int &socket, string &userid)
{
    for (auto &v : user_list)
    {
        if (v->accept == socket)
        {
            v->userid = userid;
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
    if (temp == "myuserid")
    {
        string id_str(m[0].second + 1, iterEnd);
        setuserid(sock_accept, id_str);
    }
    else if (temp == "Getroom")
        Get_hall_room(sock_accept, s);
    else if (temp == "Getuser")
        Get_hall_user(sock_accept, s);
    else if (temp == "HallSend")
    {
        string send_str(m[0].second + 1, iterEnd);
        Hall_Message(sock_accept, send_str);
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
    return "NULL";
}