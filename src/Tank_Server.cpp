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

int Get_Header_Type_bystring(string &str)
{
    return 0;
}

void send_string(int socket, string s)
{
    Header header;
    header.type = Get_Header_Type_bystring(s);
    if (header.type == 0)
        return;
    header.length = 0;
    char send_buf[sizeof(Header)] = {'\0'};
    memcpy(send_buf, &header, sizeof(Header));
    send(socket, send_buf, sizeof(Header), 0);
}

void Hall_Message(int sock_accept, Header &header, char *content)
{
    Message::Hall_Message_Request Req;
    Req.ParseFromArray(content, header.length);

    Message::Hall_Message_Response Res;

    Res.set_content(Req.content());
    Res.set_name(get_userid(sock_accept));

    for (auto &v : user_list)
    {
        if (v->states == hall)
        {
            int send_sock = v->accept;
            if (send_sock == sock_accept)
                continue;
            try
            {
                Send(send_sock, Res);
            }
            catch (exception &e)
            {
                continue;
            }
        }
    }
}

void Get_hall_info(int sock_accept)
{
    Message::Hall_info_Response Res;

    for (auto &v : user_list)
    {
        if (v->accept == sock_accept || v->states != hall)
            continue;
        Message::Hall_info_Response_User *info = Res.add_userinfo();
        info->set_name(v->userid);
    }
    for (auto &v : room_list)
    {
        if (!(v->gaming))
        {
            Message::Hall_info_Response_Roominfo *info = Res.add_roominfo();
            info->set_room_id(v->room_id);
            info->set_host_name(get_userid(v->socket_host));
        }
    }

    Send(sock_accept, Res);
}

void Create_Room(int sock_accept)
{
    Room_Process *newroom = new Room_Process(sock_accept);
    // room_info newroom;
    room_counter++;
    newroom->room_id = room_counter;
    room_list.emplace_back(newroom);
    room_user.emplace_back(sock_accept);

    thread T(&Room_Process::run, newroom);
    T.detach();
    newroom->Add_player(sock_accept);
}

void Enter_Room(int sock_accept, Header &header, char *content)
{
    Message::Hall_EnterRoom_Request Req;
    Req.ParseFromArray(content, header.length);
    int id = Req.room_id();

    Message::Hall_EnterRoom_Response Res;
    Res.set_room_id(id);
    for (auto &v : room_list)
    {
        if (v->room_id == id) //房间可加入
        {
            if (v->user_count < v->user_limited)
            {
                Res.set_result(1);
                Send(sock_accept, Res);
                v->Add_player(sock_accept);
            }
            else //房间人数已满
            {
                Res.set_result(0);
                Send(sock_accept, Res);
            }
        }
        else //房间不存在/已解散
        {
            Res.set_result(-1);
            Send(sock_accept, Res);
        }
    }
}

void set_user_id(int &socket, Header &header, char *content)
{
    Message::Set_User_id Req;
    Req.ParseFromArray(content, header.length);
    for (auto &v : user_list)
    {
        if (v->accept == socket)
        {
            v->userid = Req.name();
        }
    }
}

void return_class(int &sock_accept, Header &header, char *content)
{
    switch (header.type)
    {
    case 101:
    {
        Get_hall_info(sock_accept);
        break;
    }
    case 103:
    {
        Hall_Message(sock_accept, header, content);
        break;
    }
    case 104:
    {
        Create_Room(sock_accept);
        break;
    }
    case 105:
    {
        Enter_Room(sock_accept, header, content);
        break;
    }
    case 106:
    {
        header.type = 206;
        char buf[sizeof(Header) + header.length] = {'\0'};
        memcpy(buf, &header, sizeof(Header));
        memcpy(buf + sizeof(Header), content, header.length);
        Message::Ping_info P;
        P.ParseFromArray(content, header.length);
        int i = P.ping_id();
        send(sock_accept, buf, sizeof(Header) + header.length, 0);
        break;
    }
    case 107:
    {
        set_user_id(sock_accept, header, content);
        break;
    }
    }
}

// string return_class(int &sock_accept, string &s)
// {
//     string::const_iterator iterStart = s.begin();
//     string::const_iterator iterEnd = s.end();
//     smatch m;
//     regex reg("^[A-Z|a-z]+");
//     regex_search(iterStart, iterEnd, m, reg);
//     string temp;
//     temp = m[0];
//     if (temp == "ping")
//     {
//         send(sock_accept, (const char *)&(s[0]), 1023, 0);
//     }
//     if (temp == "myuserid")
//     {
//         string id_str(m[0].second + 1, iterEnd);
//         setuserid(sock_accept, id_str);
//     }
//     else if (temp == "Getroom")
//         Get_hall_room(sock_accept, s);
//     else if (temp == "Getuser")
//         Get_hall_user(sock_accept, s);
//     else if (temp == "HallSend")
//     {
//         string send_str(m[0].second + 1, iterEnd);
//         Hall_Message(sock_accept, send_str);
//     }
//     else if (s == "CreateRoom")
//     {
//         Create_Room(sock_accept);
//     }
//     else if (temp == "EnterRoom")
//     {
//         string send_str(m[0].second + 1, iterEnd);
//         Enter_Room(sock_accept, send_str);
//     }
//     return "NULL";
// }
