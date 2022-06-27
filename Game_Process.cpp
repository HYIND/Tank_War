#include "Game_Process.h"

Game_Process::Game_Process(int socket1, int socket2, room_info *roominfo)
    : socket1(socket1), socket2(socket2), roominfo(roominfo)
{
    Tank1 = new Tank();
    Tank2 = new Tank();
    Tank_info[socket1] = Tank1;
    Tank_info[socket2] = Tank2;

    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, recv_pipe) != -1);
    setnonblocking(recv_pipe[1]);
    game_pipe_list.emplace_back(recv_pipe[1]);

    recv_epoll = epoll_create(50);

    addfd(recv_epoll, recv_pipe[0]);
    addfd(recv_epoll, socket1);
    addfd(recv_epoll, socket2);

    memset(buffer, '\0', 1024);

    bool stop1 = false;
    bool stop2 = false;
    bool stop = false;
}

void Game_Process::run()
{
    thread send_thread(&Game_Process::send_process, this);
    thread game_thread(&Game_Process::game_process, this);
    thread recv_thread(&Game_Process::recv_process, this);
    recv_thread.join();
    game_thread.join();
    send_thread.join();
}

void Game_Process::recv_process()
{
    while (!stop)
    {
        int num = epoll_wait(recv_epoll, events, 100, -1);
        if (num < 0 && (errno != EINTR))
        {
            cout << "epoll for game failed!";
            break;
        }
        for (int i = 0; i < num; i++)
        {
            int socket = events[i].data.fd;
            if ((socket == recv_pipe[0]) && (events->events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(recv_pipe[0], signals, 1023, 0);
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
            int mysocket = socket1;
            int opsocket = socket2;
            if (socket != socket1)
                swap(mysocket, opsocket);
            if (events->events & EPOLLRDHUP)
            {
                for (auto &v : user_list)
                {
                    if (v.accept == mysocket)
                        mysocket = disconnection;
                }
                delfd(recv_epoll, socket);
                close(socket);
                two_user1[socket1] = 0;
                two_user2[socket2] = 0;
                if (mysocket == socket1)
                {
                    roominfo = NULL;
                    auto it = find(room_user.begin(), room_user.end(), socket1);
                    if (it != room_user.end())
                    {
                        room_user.erase(it);
                    }
                    for (auto it = room_list.begin(); it != room_list.end(); it++)
                    {
                        if (it->user1 == socket1)
                        {
                            room_list.erase(it);
                        }
                    }
                    stop1 = true;
                    if (!stop2)
                    {
                        for (auto &v : user_list)
                        {
                            if (v.accept == socket2 && v.states == room)
                            {
                                v.states = hall;
                                string str = "disband";
                                send(socket2, (const char *)&(str[0]), 1023, 0);
                            }
                        }
                    }
                }
                else if (mysocket == socket2)
                {
                    for (auto &v : room_list)
                        if (v.user2 == mysocket)
                            v.user2 = 0;
                    stop2 = true;
                }
                if (stop1 == true && stop2 == true)
                    stop = true;
            }
            else if (events->events & EPOLLIN)
            {
                int re_num = recv(socket, buffer, 1023, 0);
                if (re_num > 0)
                {
                    socket_messageinfo *pinfo = new socket_messageinfo(mysocket, buffer);
                    recv_queue.emplace(pinfo);
                    memset(buffer, '\0', 1024);
                    process_cv.notify_one();
                }
                else if (re_num == 0)
                {
                    for (auto &v : user_list)
                    {
                        if (v.accept == mysocket)
                            mysocket = disconnection;
                    }
                    delfd(recv_epoll, socket);
                    close(socket);
                    two_user1[socket1] = 0;
                    two_user2[socket2] = 0;
                    if (mysocket == socket1)
                    {
                        roominfo = NULL;
                        auto it = find(room_user.begin(), room_user.end(), socket1);
                        if (it != room_user.end())
                        {
                            room_user.erase(it);
                        }
                        for (auto it = room_list.begin(); it != room_list.end(); it++)
                        {
                            if (it->user1 == socket1)
                            {
                                room_list.erase(it);
                            }
                        }
                        stop1 = true;
                        if (!stop2)
                        {
                            for (auto &v : user_list)
                            {
                                if (v.accept == socket2 && v.states == room)
                                {
                                    v.states = hall;
                                    string str = "disband";
                                    send(socket2, (const char *)&(str[0]), 1023, 0);
                                }
                            }
                        }
                    }
                    else if (mysocket == socket2)
                    {
                        for (auto &v : room_list)
                            if (v.user2 == mysocket)
                                v.user2 = 0;
                        stop2 = true;
                    }
                    if (stop1 == true && stop2 == true)
                        stop = true;
                }
            }
        }
    }
    for (auto it = game_pipe_list.begin(); it != game_pipe_list.end(); it++)
    {
        if (*it == recv_pipe[1])
        {
            game_pipe_list.erase(it);
            break;
        }
    }
    close(recv_pipe[0]);
    close(recv_pipe[1]);
    Tank_info[socket1] == NULL;
    Tank_info[socket2] == NULL;
    delete (Tank1);
    delete (Tank2);
}

void Game_Process::game_process()
{
    socket_messageinfo *info = NULL;
    int mysocket;
    int opsocket;
    string str;
    char *buf;
    while (!stop)
    {
        bool release = false;
        unique_lock<mutex> lck(process_mtx);
        process_cv.wait(lck);
        while (!recv_queue.empty())
        {
            info = recv_queue.front();
            recv_queue.pop();

            mysocket = info->socket;
            opsocket = (mysocket == socket1) ? socket2 : socket1;
            str = info->ch;
            buf = info->ch;
            string::const_iterator iterStart = str.begin();
            string::const_iterator iterEnd = str.end();
            smatch m;
            regex reg("^[A-Z|a-z]+");
            regex_search(iterStart, iterEnd, m, reg);
            string option;
            option = m[0];
            if (option == "returntoroom")
            {
                try
                {
                    delfd(recv_epoll, mysocket);
                    if (mysocket == socket1)
                    {
                        for (auto &v : user_list)
                        {
                            if (v.accept == mysocket)
                            {
                                v.states = room;
                            }
                            if (stop2)
                            {
                                stop = true;
                            }
                        }
                    }
                    else if (mysocket == socket2)
                    {
                        for (auto &v : user_list)
                        {
                            if (v.accept == mysocket)
                            {
                                if (roominfo)
                                {
                                    v.states = room;
                                    if (stop1)
                                    {
                                        stop = true;
                                    }
                                }
                                else
                                {
                                    v.states = hall;
                                    string str = "disband";
                                    send(socket2, (const char *)&(str[0]), 1023, 0);
                                    stop = true;
                                }
                            }
                        }
                    }
                    addfd(hall_epoll, mysocket);
                    break;
                }
                catch (const std::exception &e)
                {
                    break;
                }
            }
            else if (option == "ping")
            {
                send_queue.emplace(info);
                // string s = buf;
                // send(mysocket, (const char *)buf, 1023, 0);
            }
            else if (option == "mylocation")
            {
                try
                {
                    Tank *mytank = Tank_info[mysocket];
                    if (mytank->isalive)
                    {
                        memcpy(mytank, &buf[11], 24);
                        // char send_ch[1024] = "oplocation:";
                        // memcpy(&send_ch[11], &buf[11], 24);
                        buf[0] = 'o';
                        buf[1] = 'p';
                        info->socket = opsocket;
                        send_queue.emplace(info);
                        // send(opsocket, (const char *)&(buf[0]), 1023, 0);
                        // memset(buf, '\0', 1024);
                    }
                }
                catch (exception &e)
                {
                    return;
                }
            }
            else if (option == "mybullet")
            {
                // char send_ch[1024] = "opbullet:";
                // memcpy(&send_ch[9], &buf[9], 1011);
                buf[0] = 'o';
                buf[1] = 'p';
                info->socket = opsocket;
                send_queue.emplace(info);
                // send(opsocket, (const char *)&(buf[0]), 1023, 0);
            }
            // destory:{111,222}
            else if (option == "destroy")
            {
                Tank *optank = Tank_info[opsocket];
                if (optank->isalive)
                {
                    try
                    {
                        regex loc_reg("[0-9]+");
                        string temp = buf;
                        string::const_iterator iterStart = temp.begin();
                        string::const_iterator iterEnd = temp.end();
                        sregex_iterator iter(iterStart, iterEnd, loc_reg);
                        string s_loc_X = ((*iter)[0]);
                        iter++;
                        string s_loc_Y = ((*iter)[0]);
                        int loc_X = atoi(s_loc_X.c_str());
                        int loc_Y = atoi(s_loc_Y.c_str());

                        int half_height = optank->height / 2;
                        int half_width = optank->width / 2;
                        if (loc_Y < optank->locationY + half_height && loc_Y > optank->locationY - half_height && loc_X < optank->locationX + half_width && loc_X > optank->locationX - half_width)
                        {
                            optank->isalive = false;

                            string str = "destroy";
                            char ch1[1024] = {'\0'};
                            strcpy(ch1, str.c_str());
                            socket_messageinfo *info1 = new socket_messageinfo(opsocket, ch1);
                            send_queue.emplace(info1);

                            str = "opdestroy";
                            char ch2[1024] = {'\0'};
                            strcpy(ch2, str.c_str());
                            socket_messageinfo *info2 = new socket_messageinfo(mysocket, ch2);
                            send_queue.emplace(info2);

                            release = true;

                            // send(opsocket, (const char *)&(str[0]), 1023, 0);
                            // send(mysocket, (const char *)&(str[0]), 1023, 0);
                            // memset(buf, '\0', 1024);
                        }
                    }
                    catch (exception &e)
                    {
                        return;
                    }
                }
            }
            send_cv.notify_one();
            if (release)
            {
                delete (info);
            }
        }
        lck.unlock();
    }
}

void Game_Process::send_process()
{
    socket_messageinfo *info = NULL;
    while (!stop)
    {
        unique_lock<mutex> lck(send_mtx);
        send_cv.wait(lck);
        while (!send_queue.empty())
        {
            info = send_queue.front();
            send_queue.pop();
            send(info->socket, (const char *)info->ch, 1023, 0);

            delete (info);
        }
        lck.unlock();
    }
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