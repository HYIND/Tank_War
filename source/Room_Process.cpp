#include "Room_Game_Process.h"

// Room_Process::Room_Process(int socket1, int socket2, room_info *roominfo)
//     : socket1(socket1), socket2(socket2), roominfo(roominfo)
// {
//     Tank1 = new Tank();
//     Tank2 = new Tank();
//     Tank_info[socket1] = Tank1;
//     Tank_info[socket2] = Tank2;

//     assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, recv_pipe) != -1);
//     setnonblocking(recv_pipe[1]);
//     game_pipe_list.emplace_back(recv_pipe[1]);

//     recv_epoll = epoll_create(50);

//     addfd(recv_epoll, recv_pipe[0]);
//     addfd(recv_epoll, socket1);
//     addfd(recv_epoll, socket2);

//     memset(buffer, '\0', 1024);

//     bool stop1 = false;
//     bool stop2 = false;
//     bool stop = false;
// }

Room_Process::Room_Process(int socket)
    : socket_host(socket)
{
    assert(socketpair(PF_UNIX, SOCK_STREAM, 0, recv_pipe) != -1);
    setnonblocking(recv_pipe[1]);
    game_pipe_list.emplace_back(recv_pipe[1]);

    recv_epoll = epoll_create(50);

    addfd(recv_epoll, recv_pipe[0], false);

    memset(buffer, '\0', 1024);

    bool stop = false;

    Set_Map(0);
}

Room_Process::~Room_Process()
{
    //置stop位，停线程
    stop = true;
    char ch[] = "kill";
    int i = send(recv_pipe[1], ch, 4, 0);
    process_cv.notify_one();
    send_cv.notify_one();

    //回收Room_user信息
    for (auto &v : info)
    {
        delfd(recv_epoll, v.first);
        addfd(hall_epoll, v.first);
        v.second->sockinfo->states = hall;
        delete v.second;
    }
    //回收Tank信息
    for (auto &v : Tank_info)
    {
        delete v.second;
    }
    //回收消息队列中的消息
    while (!recv_queue.empty())
    {
        delete recv_queue.front();
        recv_queue.pop();
    }
    while (!send_queue.empty())
    {
        delete send_queue.front();
        send_queue.pop();
    }
    //移除管道
    for (auto it = game_pipe_list.begin(); it != game_pipe_list.end(); it++)
    {
        if (*it == this->recv_pipe[1])
        {
            game_pipe_list.erase(it);
            break;
        }
    }
    //关闭管道
    close(recv_pipe[0]);
    close(recv_pipe[1]);
    //把该房间从room_list移除
    for (auto it = room_list.begin(); it != room_list.end(); it++)
    {
        if ((*it)->room_id == this->room_id)
        {
            room_list.erase(it);
            break;
        }
    }
}

void Room_Process::Add_player(int socket)
{
    for (auto &v : user_list)
    {
        if (v->accept == socket)
        {
            v->states = room;
            Room_userinfo *userinfo = new Room_userinfo();
            userinfo->sockinfo = v;
            unique_lock<mutex> infolck(info_mtx);
            info[socket] = userinfo;
            infolck.unlock();
            break;
        }
    }
    delfd(hall_epoll, socket);
    addfd(recv_epoll, socket);
    user_count++;
    info[socket]->tank_id = user_count;
    string send_str = "tankid:" + to_string(user_count);
    char send_ch[1024] = {'\0'};
    strcpy(send_ch, send_str.c_str());
    socket_messageinfo *pmsginfo = new socket_messageinfo(socket, send_ch);
    unique_lock<mutex> slck(sendqueue_mtx);
    send_queue.emplace(pmsginfo);
    sendqueue_mtx.unlock();
}

void Room_Process::run()
{
    thread send_thread(&Room_Process::send_process, this);
    thread room_thread(&Room_Process::room_process, this);
    thread recv_thread(&Room_Process::recv_process, this);
    room_thread.detach();
    recv_thread.join();
    send_thread.join();
}

void Room_Process::recv_process()
{
    int re_num = 0;
    while (!stop)
    {
        int num = epoll_wait(recv_epoll, events, 100, 30);
        if (stop)
            break;
        if (num < 0 && (errno != EINTR))
        {
            cout << "epoll for game failed!";
            break;
        }
        for (int i = 0; i < num; i++)
        {
            int socket = events[i].data.fd;
            if ((socket == recv_pipe[0]) && (events[i].events & EPOLLIN))
            {
                break;
                // int sig;
                // char signals[1024];
                // int ret = recv(recv_pipe[0], signals, 1023, 0);
                // if (ret == -1 || ret == 0)
                //     continue;
                // else
                // {
                //     for (i = 0; i < ret; i++)
                //     {
                //         switch (signals[i])
                //         {
                //         case SIGINT:
                //         case SIGTERM:
                //         {
                //             stop = true;
                //         }
                //         }
                //     }
                // }
            }
            // int mysocket = socket_host;
            // int opsocket = socket2;
            if (events[i].events & EPOLLRDHUP)
            {
                stop = true;
                //非正常退出，暂时直接回收资源
                delete (this);
                break;
            }
            else if (events[i].events & EPOLLIN)
            {
                re_num = recv(socket, buffer, 1023, 0);
                while (re_num > 0)
                {
                    if (stop)
                        break;
                    socket_messageinfo *pinfo = new socket_messageinfo(socket, buffer);
                    unique_lock<mutex> rlck(recvqueue_mtx);
                    recv_queue.emplace(pinfo);
                    rlck.unlock();
                    memset(buffer, '\0', 1024);
                    process_cv.notify_one();
                    re_num = recv(socket, buffer, 1023, 0);
                }
                if (re_num == 0)
                {
                    // for (auto &v : user_list)
                    // {
                    //     if (v.accept == mysocket)
                    //         mysocket = disconnection;
                    // }
                    // delfd(recv_epoll, socket);
                    // close(socket);
                    // two_user1[socket1] = 0;
                    // two_user2[socket2] = 0;
                    // if (mysocket == socket1)
                    // {
                    //     roominfo = NULL;
                    //     auto it = find(room_user.begin(), room_user.end(), socket1);
                    //     if (it != room_user.end())
                    //     {
                    //         room_user.erase(it);
                    //     }
                    //     for (auto it = room_list.begin(); it != room_list.end(); it++)
                    //     {
                    //         if (it->user1 == socket1)
                    //         {
                    //             room_list.erase(it);
                    //         }
                    //     }
                    //     stop1 = true;
                    //     if (!stop2)
                    //     {
                    //         for (auto &v : user_list)
                    //         {
                    //             if (v.accept == socket2 && v.states == room)
                    //             {
                    //                 v.states = hall;
                    //                 string str = "disband";
                    //                 send(socket2, (const char *)&(str[0]), 1023, 0);
                    //             }
                    //         }
                    //     }
                    // }
                    // else if (mysocket == socket2)
                    // {
                    //     for (auto &v : room_list)
                    //         if (v.user2 == mysocket)
                    //             v.user2 = 0;
                    //     stop2 = true;
                    // }
                    // if (stop1 == true && stop2 == true)
                    //     stop = true;
                }
            }
        }
    }
}

void Room_Process::room_process()
{
    socket_messageinfo *messageinfo = NULL;
    bool RP_stop = false;
    string str;
    string ret;
    while (!RP_stop)
    {
        // if (info[socket_host] == NULL)
        // {
        //     ret == "disband";
        //     break;
        // }
        unique_lock<mutex> lck(process_mtx);
        process_cv.wait(lck);
        while (!recv_queue.empty())
        {
            if (stop)
            {
                break;
            }
            unique_lock<mutex> qlck(recvqueue_mtx);
            messageinfo = recv_queue.front();
            recv_queue.pop();
            qlck.unlock();

            int socket = messageinfo->socket;
            str = messageinfo->ch;

            ret = return_class(socket, str);
            if (ret == "StartGame" || ret == "disband")
            {
                RP_stop = true;
                break;
            }
            send_cv.notify_one();
        }
        lck.unlock();
    }
    if (ret == "StartGame")
    {
        thread T(&Room_Process::game_process, this);
        T.detach();
    }
    if (ret == "disband")
    {
        delete (this);
    }
}

string Room_Process::return_class(int socket, string &str)
{
    string::const_iterator iterStart = str.begin();
    string::const_iterator iterEnd = str.end();
    smatch m;
    regex reg("^[A-Z|a-z]+");
    regex_search(iterStart, iterEnd, m, reg);
    string temp;
    temp = m[0];
    if (temp == "ping")
    {
        send(socket, (const char *)&(str[0]), 1023, 0);
    }
    else if (temp == "GetRoominfo")
    {
        Return_Room_info(socket);
    }
    else if (temp == "Ready")
    {
        Ready(socket);
    }
    else if (temp == "CancelReady")
    {
        Cancel_Ready(socket);
    }
    else if (temp == "RoomSend")
    {
        string send_str(m[0].second + 1, iterEnd);
        Room_Message(socket, send_str);
    }
    else if (temp == "QuitRoom")
    {
        return Quit_Room(socket);
    }
    else if (temp == "StartGame")
    {
        return Start_Game(socket);
    }
    else if (temp == "ChangeMap")
    {
        string send_str(m[0].second + 1, iterEnd);
        Change_Map(socket, send_str);
    }
    return "NULL";
}

void Room_Process::Set_Map(int id)
{
    this->map_id = id;
    this->map_info = Map_list[id];
}

void Room_Process::Change_Map(int socket, string &recv_str) //房主切换地图
{
    if (socket != socket_host)
        return;
    string new_id_str;
    try
    {
        string::const_iterator iterStart = recv_str.begin();
        string::const_iterator iterEnd = recv_str.end();
        smatch m;
        regex id_reg("[0-9]+");
        regex_search(iterStart, iterEnd, m, id_reg);
        new_id_str = m[0];
        int new_map_id = atoi(new_id_str.c_str());
        Set_Map(new_map_id);
    }
    catch (exception &e)
    {
        return;
    }
    string send_str = "ChangeMapto:" + new_id_str;
    char send_ch[1024] = {'\0'};
    strcpy(send_ch, send_str.c_str());
    for (auto &v : info)
    {
        socket_messageinfo *pmsginfo = new socket_messageinfo(socket, send_ch);
        unique_lock<mutex> slck(sendqueue_mtx);
        send_queue.emplace(pmsginfo);
        sendqueue_mtx.unlock();
    }
}

void Room_Process::Ready(int socket)
{
    unique_lock<mutex> infolck(info_mtx);
    info[socket]->Ready = true;
    infolck.unlock();
    return;
};
void Room_Process::Cancel_Ready(int socket)
{
    unique_lock<mutex> infolck(info_mtx);
    info[socket]->Ready = false;
    infolck.unlock();
    return;
}

string Room_Process::Start_Game(int socket)
{
    //检查是否房主
    if (socket != socket_host)
    {
        return "NULL";
    }
    //检查人数
    if (info.size() < 2)
    {
        return "NULL";
    }

    unique_lock<mutex> infolck(info_mtx);
    info[socket_host]->Ready = true;
    //检查是否都已经准备
    for (auto &v : info)
    {
        if (v.second->Ready == false)
        {
            info[socket_host]->Ready = false;
            infolck.unlock();
            return "NULL";
            // send()
        }
    }
    //变更用户状态
    for (auto &v : info)
    {
        v.second->sockinfo->states = gaming;
    }
    infolck.unlock();

    //返回启动game_process信息
    return "StartGame";
}

string Room_Process::Quit_Room(int socket)
{
    //房主退出，房间解散
    if (socket == socket_host)
    {
        string str = "disband";
        unique_lock<mutex> infolck(info_mtx);
        for (auto &v : info)
        {
            if (v.first != socket_host)
            {
                v.second->sockinfo->states = hall;
                send(v.first, (const char *)&str[0], 1023, 0);
            }
        }
        infolck.unlock();
        return str;
    }
    //非房主退出
    else
    {
        unique_lock<mutex> infolck(info_mtx);
        info[socket]->sockinfo->states = hall;
        delfd(recv_epoll, socket);
        addfd(hall_epoll, socket);
        info.erase(socket);
        infolck.unlock();
        return "NULL";
    }
}

void Room_Process::Room_Message(int socket, string &send_str)
{
    unique_lock<mutex> infolck1(info_mtx);
    send_str = "RoomMessage:" + info[socket]->sockinfo->userid + "_content:" + send_str;
    infolck1.unlock();
    char send_ch[1024] = {'\0'};
    strcpy(send_ch, send_str.c_str());

    unique_lock<mutex> infolck2(info_mtx);
    for (auto &v : info)
    {
        if (v.first == socket)
            continue;
        try
        {
            socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, send_ch);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(pmsginfo);
            sendqueue_mtx.unlock();
            // send(socket, (const char *)&(send_str[0]), 1023, 0);
        }
        catch (exception &e)
        {
            continue;
        }
    }
    infolck2.unlock();
}
void Room_Process::Return_Room_info(int socket)
{
    string send_str = "Roomuser:";
    unique_lock<mutex> infolck(info_mtx);
    for (auto &v : info)
    {
        if (v.first == socket)
            continue;
        send_str += "#";
        send_str = send_str + v.second->sockinfo->userid;
        if (v.first == socket_host)
            send_str += "::2;";
        else if (v.second->Ready)
            send_str += "::1;";
        else
            send_str += "::0;";
    }
    infolck.unlock();

    char send_ch[1024] = {'\0'};
    strcpy(send_ch, send_str.c_str());
    socket_messageinfo *pmsginfo = new socket_messageinfo(socket, send_ch);
    unique_lock<mutex> slck(sendqueue_mtx);
    send_queue.emplace(pmsginfo);
    sendqueue_mtx.unlock();
    // send(socket, (const char *)&(send_str[0]), 1023, 0);
}

void Room_Process::send_process()
{
    socket_messageinfo *info = NULL;
    while (!stop)
    {
        unique_lock<mutex> lck(send_mtx);
        send_cv.wait(lck);
        while (!send_queue.empty())
        {
            if (stop)
            {
                break;
            }
            unique_lock<mutex> slck(sendqueue_mtx);
            info = send_queue.front();
            send_queue.pop();
            sendqueue_mtx.unlock();
            try
            {
                signal(SIGPIPE, SIG_IGN);
                send(info->socket, (const char *)info->ch, 1023, 0);
            }
            catch (const exception &e)
            {
                continue;
            }
            delete (info);
        }
        lck.unlock();
    }
    int i = 0;
}

// void return_game_class(int mysocket, int opsocket, string &option, char buf[])
// {
//     if (option == "ping")
//     {
//         string s = buf;
//         send(mysocket, (const char *)buf, 1023, 0);
//     }
//     if (option == "mylocation")
//     {
//         try
//         {
//             Tank *mytank = Tank_info[mysocket];
//             if (mytank->isalive)
//             {
//                 memcpy(mytank, &buf[11], 24);
//                 char send_ch[1024] = "oplocation:";
//                 memcpy(&send_ch[11], &buf[11], 24);
//                 send(opsocket, (const char *)&(send_ch[0]), 1023, 0);
//                 memset(buf, '\0', 1024);
//             }
//         }
//         catch (exception &e)
//         {
//             return;
//         }
//     }
//     else if (option == "mybullet")
//     {
//         char send_ch[1024] = "opbullet:";
//         memcpy(&send_ch[9], &buf[9], 1011);
//         send(opsocket, (const char *)&(send_ch[0]), 1023, 0);
//     }
//     // destory:{111,222}
//     else if (option == "destroy")
//     {
//         Tank *optank = Tank_info[opsocket];
//         if (optank->isalive)
//         {
//             try
//             {
//                 regex loc_reg("[0-9]+");
//                 string temp = buf;
//                 string::const_iterator iterStart = temp.begin();
//                 string::const_iterator iterEnd = temp.end();
//                 sregex_iterator iter(iterStart, iterEnd, loc_reg);
//                 string s_loc_X = ((*iter)[0]);
//                 iter++;
//                 string s_loc_Y = ((*iter)[0]);
//                 int loc_X = atoi(s_loc_X.c_str());
//                 int loc_Y = atoi(s_loc_Y.c_str());
//                 int half_height = optank->height / 2;
//                 int half_width = optank->width / 2;
//                 if (loc_Y < optank->locationY + half_height && loc_Y > optank->locationY - half_height && loc_X < optank->locationX + half_width && loc_X > optank->locationX - half_width)
//                 {
//                     optank->isalive = false;
//                     string str = "destroy";
//                     send(opsocket, (const char *)&(str[0]), 1023, 0);
//                     str = "opdestroy";
//                     send(mysocket, (const char *)&(str[0]), 1023, 0);
//                     memset(buf, '\0', 1024);
//                 }
//             }
//             catch (exception &e)
//             {
//                 return;
//             }
//         }
//     }
// }