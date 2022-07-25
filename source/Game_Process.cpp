#include "Room_Game_Process.h"

bool collision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    //检测Y轴碰撞,碰撞则返回true;
    bool collisionY = y1 - height1 / 2 < y2 + height2 / 2 == y1 + height1 / 2 > y2 - height2 / 2;
    //检测X轴碰撞
    bool collisionX = x1 - width1 / 2 < x2 + width2 / 2 == x1 + width1 / 2 > x2 - width2 / 2;

    if (collisionX && collisionY)
    {
        return true;
    }
    return false;
}

void Room_Process::confim()
{
    clock_t start, now, timeout;
    timeout = 20 * CLOCKS_PER_SEC;

    map<int, bool> confim;
    string send_str = "ConfimMapid:" + to_string(map_id);
    for (auto &v : info)
    {
        confim[v.first] = false;
        send(v.first, (const char *)&(send_str[0]), 1023, 0);
    }

    socket_messageinfo *messageinfo = nullptr;
    string str;
    char *buf;
    int cur_socket;
    start = clock();
    while (!recv_queue.empty())
    {
        now = clock();
        if (now - start > timeout)
            break;
        unique_lock<mutex> qlck(recvqueue_mtx);
        messageinfo = recv_queue.front();
        recv_queue.pop();
        qlck.unlock();
        cur_socket = messageinfo->socket;
        buf = messageinfo->ch;
        str = messageinfo->ch;
        string::const_iterator iterStart = str.begin();
        string::const_iterator iterEnd = str.end();
        smatch m;
        regex reg("^[A-Z|a-z]+");
        regex_search(iterStart, iterEnd, m, reg);
        string option;
        option = m[0];
        if (option == "ConfimMapid")
        {
            try
            {
                string recv_id(m[0].second + 1, iterEnd);
                if (map_id == atoi(recv_id.c_str()))
                {
                    confim[cur_socket] = true;
                }
            }
            catch (exception &e)
            {
                continue;
            }
        }
        delete messageinfo;
        messageinfo = nullptr;
    }
    for (auto &v : confim)
    {
        if (v.second == false)
        {
            info.erase(v.first);
            for (auto it = user_list.begin(); it != user_list.end(); it++)
            {
                if ((*it)->accept == v.first)
                {
                    user_list.erase(it);
                    break;
                }
            }
            user_count--;
        }
    }
}

void Room_Process::Init_Game()
{
    this->map_info = Map_list[map_id];

    //服务端初始化Tank信息
    for (auto &v : info)
    {
        Tank *tank = new Tank();
        for (auto &m : map_info.Init_Location)
        {
            if (m.Tank_id == v.second->tank_id)
            {
                tank->locationX = m.x;
                tank->locationY = m.y;
                tank->width = Tank_Style_info[m.tank_style]->width;
                tank->height = Tank_Style_info[m.tank_style]->height;
                tank->direction = m.direction;
                tank->isalive = m.isalive;
            }
        }
        Tank_info[v.first] = tank;
    }

    string send_str = "Start"; //开始游戏
    for (auto &v : info)
    {
        send(v.first, (const char *)&(send_str[0]), 1023, 0);
    }
}

void Room_Process::EndGame()
{
    char ch_win[1024] = "wingame";
    char ch_fail[1024] = "failgame";
    for (auto &v : info)
    {
        if (Tank_info[v.first]->isalive)
        {
            socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, ch_win);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(pmsginfo);
            sendqueue_mtx.unlock();
        }
        else
        {
            socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, ch_fail);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(pmsginfo);
            sendqueue_mtx.unlock();
        }
    }
}

bool Room_Process::hittank(char buf[])
{
    try
    {
        int hited_tank_id;
        int cur_loc = 8;
        if (buf[cur_loc] == '(' && buf[cur_loc + sizeof(int) + 1] == ')')
        {
            cur_loc++;
            memcpy(&hited_tank_id, &buf[cur_loc], sizeof(int));
        }
        else
            return false;
        cur_loc += sizeof(int) + 1;
        int hited_socket;
        for (auto &v : info)
        {
            if (v.second->tank_id == hited_tank_id)
            {
                hited_socket = v.first;
                break;
            }
        }
        Tank *hited_tank = Tank_info[hited_socket];
        int bullet_x, bullet_y, direction;
        BulletStyle style;
        if (buf[cur_loc] == '{' && buf[cur_loc + sizeof(int) * 3 + sizeof(BulletStyle) + 1] == '}')
        {
            cur_loc++;
            memcpy(&bullet_x, &buf[cur_loc], sizeof(int));
            cur_loc += sizeof(int);
            memcpy(&bullet_y, &buf[cur_loc], sizeof(int));
            cur_loc += sizeof(int);
            memcpy(&direction, &buf[cur_loc], sizeof(int));
            cur_loc += sizeof(int);
            memcpy(&style, &buf[cur_loc], sizeof(BulletStyle));
            cur_loc += sizeof(BulletStyle);
        }
        int width = Bullet_Style_info[style]->width;
        if (collision(bullet_x,
                      bullet_y,
                      Bullet_Style_info[style]->width,
                      Bullet_Style_info[style]->height,
                      hited_tank->locationX,
                      hited_tank->locationY,
                      hited_tank->width,
                      hited_tank->height))
        {
            hited_tank->health -= 21;
            hittank_notify(hited_socket, hited_tank_id);
            if (hited_tank->health <= 0)
            {
                hited_tank->isalive = false;
                destroy(hited_socket, hited_tank_id);
                return true;
            }
        }
        return false;
    }
    catch (exception &e)
    {
        return false;
    }
}

bool Room_Process::hitbrick(char buf[])
{
    try
    {
        int hited_brick_id;
        int cur_loc = 9;
        if (buf[cur_loc] == '(' && buf[cur_loc + sizeof(int) + 1] == ')')
        {
            cur_loc++;
            memcpy(&hited_brick_id, &buf[cur_loc], sizeof(int));
        }
        else
            return false;
        cur_loc += sizeof(int) + 1;
        auto iter = map_info.Brick_info.begin();
        while (iter != map_info.Brick_info.end())
        {
            if (iter->id == hited_brick_id)
                break;
            iter++;
        }
        if (iter == map_info.Brick_info.end())
            return false;
        Brick_Wall *pwall = &(*iter);
        int bullet_x, bullet_y, direction;
        BulletStyle style;
        if (buf[cur_loc] == '{' && buf[cur_loc + sizeof(int) * 3 + sizeof(BulletStyle) + 1] == '}')
        {
            cur_loc++;
            memcpy(&bullet_x, &buf[cur_loc], sizeof(int));
            cur_loc += sizeof(int);
            memcpy(&bullet_y, &buf[cur_loc], sizeof(int));
            cur_loc += sizeof(int);
            memcpy(&direction, &buf[cur_loc], sizeof(int));
            cur_loc += sizeof(int);
            memcpy(&style, &buf[cur_loc], sizeof(BulletStyle));
            cur_loc += sizeof(BulletStyle);
        }
        if (collision(bullet_x,
                      bullet_y,
                      Bullet_Style_info[style]->width,
                      Bullet_Style_info[style]->height,
                      pwall->locationX,
                      pwall->locationY,
                      pwall->width,
                      pwall->height))
        {
            pwall->health -= 21;
            hitbrick_notify(hited_brick_id, pwall->health);
            if (pwall->health <= 0)
            {
                map_info.Brick_info.erase(iter);
                return true;
            }
        }
        return false;
    }
    catch (exception &e)
    {
        return false;
    }
}

void Room_Process::hittank_notify(int hited_socket, int hited_id)
{
    static const string str_hited = "youhited";
    static const string str_other = "hited:";
    char ch_hited[1024] = {'\0'};
    char ch_other[1024] = {'\0'};

    strcpy(ch_hited, str_hited.c_str());

    strcpy(ch_other, str_other.c_str());
    int cur_loc = 6;
    ch_other[cur_loc] = '(';
    cur_loc++;
    memcpy(&ch_other[cur_loc], &hited_id, sizeof(int));
    cur_loc += sizeof(int);
    ch_other[cur_loc] = ')';

    socket_messageinfo *pmsginfo = new socket_messageinfo(hited_socket, ch_hited);
    unique_lock<mutex> slck(sendqueue_mtx);
    send_queue.emplace(pmsginfo);
    sendqueue_mtx.unlock();
    for (auto &v : info)
    {
        if (v.first != hited_socket)
        {
            socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, ch_other);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(pmsginfo);
            sendqueue_mtx.unlock();
        }
    }
}

// hitbrick:([INT][INT])
void Room_Process::hitbrick_notify(int hited_brick_id, int health)
{
    static const string str_send = "hitbrick:";
    char ch_send[1024] = {'\0'};

    strcpy(ch_send, str_send.c_str());
    int cur_loc = 9;
    ch_send[cur_loc] = '(';
    cur_loc++;
    memcpy(&ch_send[cur_loc], &hited_brick_id, sizeof(int));
    cur_loc += sizeof(int);
    memcpy(&ch_send[cur_loc], &health, sizeof(int));
    cur_loc += sizeof(int);
    ch_send[cur_loc] = ')';

    for (auto &v : info)
    {
        socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, ch_send);
        unique_lock<mutex> slck(sendqueue_mtx);
        send_queue.emplace(pmsginfo);
        sendqueue_mtx.unlock();
    }
}

void Room_Process::destroy(int hited_socket, int hited_id)
{
    static const string str_hited = "youdestroyed";
    static const string str_other = "destroyed:";
    char ch_hited[1024] = {'\0'};
    char ch_other[1024] = {'\0'};

    strcpy(ch_hited, str_hited.c_str());

    strcpy(ch_other, str_other.c_str());
    int cur_loc = 10;
    ch_other[cur_loc] = '(';
    cur_loc++;
    memcpy(&ch_other[cur_loc], &hited_id, sizeof(int));
    cur_loc += sizeof(int);
    ch_other[cur_loc] = ')';

    socket_messageinfo *pmsginfo = new socket_messageinfo(hited_socket, ch_hited);
    unique_lock<mutex> slck(sendqueue_mtx);
    send_queue.emplace(pmsginfo);
    sendqueue_mtx.unlock();
    for (auto &v : info)
    {
        if (v.first != hited_socket)
        {
            socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, ch_other);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(pmsginfo);
            sendqueue_mtx.unlock();
        }
    }
}

void Room_Process::game_process()
{
    socket_messageinfo *messageinfo = NULL;
    string str;
    char *buf;
    int cur_socket;

    // confim();
    // if (user_count == 0)
    // {
    //     stop = true;
    // }

    Init_Game();

    bool end = false;
    int player_alive = user_count;
    while (!end && !stop)
    {
        try
        {
            unique_lock<mutex> lck(process_mtx);
            process_cv.wait(lck);
            while (!recv_queue.empty())
            {
                bool release = false;
                if (stop)
                {
                    break;
                }
                unique_lock<mutex> qlck(recvqueue_mtx);
                messageinfo = recv_queue.front();
                recv_queue.pop();
                qlck.unlock();
                cur_socket = messageinfo->socket;
                buf = messageinfo->ch;
                str = messageinfo->ch;
                string::const_iterator iterStart = str.begin();
                string::const_iterator iterEnd = str.end();
                smatch m;
                regex reg("^[A-Z|a-z]+");
                regex_search(iterStart, iterEnd, m, reg);
                string option;
                option = m[0];
                // {
                //     try
                //     {
                //         delfd(recv_epoll, mysocket);
                //         if (mysocket == socket1)
                //         {
                //             for (auto &v : user_list)
                //             {
                //                 if (v.accept == mysocket)
                //                 {
                //                     v.states = room;
                //                 }
                //             }
                //             stop1 = true;
                //             if (stop2)
                //             {
                //                 stop = true;
                //             }
                //         }
                //         else if (mysocket == socket2)
                //         {
                //             for (auto &v : user_list)
                //             {
                //                 if (v.accept == mysocket)
                //                 {
                //                     if (roominfo)
                //                     {
                //                         v.states = room;
                //                     }
                //                     else
                //                     {
                //                         v.states = hall;
                //                         string str = "disband";
                //                         send(socket2, (const char *)&(str[0]), 1023, 0);
                //                     }
                //                 }
                //             }
                //             stop2 = true;
                //             if (stop1)
                //             {
                //                 stop = true;
                //             }
                //         }
                //         addfd(hall_epoll, mysocket);
                //         break;
                //     }
                //     catch (const std::exception &e)
                //     {
                //         break;
                //     }
                // }
                if (option == "ping")
                {
                    unique_lock<mutex> slck(sendqueue_mtx);
                    send_queue.emplace(messageinfo);
                    sendqueue_mtx.unlock();
                    // string s = buf;
                    // send(mysocket, (const char *)buf, 1023, 0);
                }
                else if (option == "mytankinfo")
                {
                    try
                    {
                        Tank *mytank = Tank_info[cur_socket];
                        if (mytank->isalive)
                        {
                            memcpy(mytank, &buf[11], 21);
                            int id = info[cur_socket]->tank_id;
                            char send_ch[1024] = "tankinfo:(";
                            int cur_loc = 10;
                            memcpy(&send_ch[cur_loc], &id, sizeof(int));
                            cur_loc += sizeof(int);
                            send_ch[cur_loc] = ')';
                            cur_loc++;
                            send_ch[cur_loc] = '{';
                            cur_loc++;
                            memcpy(&send_ch[cur_loc], &buf[11], 21);
                            cur_loc += 21;
                            send_ch[cur_loc] = '}';
                            for (auto &v : info)
                            {
                                if (v.first == cur_socket)
                                    continue;
                                unique_lock<mutex> slck(sendqueue_mtx);
                                socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, send_ch);
                                send_queue.emplace(pmsginfo);
                                sendqueue_mtx.unlock();
                            }
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
                    try
                    {
                        int id = info[cur_socket]->tank_id;
                        char send_ch[1024] = "bulletinfo:(";
                        int cur_loc = 12;
                        memcpy(&send_ch[cur_loc], &id, sizeof(int));
                        cur_loc += sizeof(int);
                        if (id < 10)
                        {
                            send_ch[cur_loc] = ')';
                            cur_loc++;
                        }
                        memcpy(&send_ch[cur_loc], &buf[9], 900);
                        for (auto &v : info)
                        {
                            if (v.first == cur_socket)
                                continue;
                            unique_lock<mutex> slck(sendqueue_mtx);
                            socket_messageinfo *pmsginfo = new socket_messageinfo(v.first, send_ch);
                            send_queue.emplace(pmsginfo);
                            sendqueue_mtx.unlock();
                        }
                    }
                    catch (exception &e)
                    {
                        return;
                    }
                }
                // destroytank:(INT){INT,INT,bulletStyle}
                else if (option == "hittank")
                {
                    if (hittank(buf))
                    {
                        player_alive--;
                    }
                    release = true;
                }
                else if (option == "hitbrick")
                {
                    hitbrick(buf);
                    release = true;
                }
                else if (option == "returntohall")
                {
                    unique_lock<mutex> infolck(info_mtx);
                    info[cur_socket]->sockinfo->states = hall;
                    delfd(recv_epoll, cur_socket);
                    addfd(hall_epoll, cur_socket);
                    info.erase(cur_socket);
                    infolck.unlock();
                }
                send_cv.notify_one();
                if (release)
                {
                    delete (messageinfo);
                }
                messageinfo = NULL;
                if (player_alive == 1)
                {
                    EndGame();
                    end = true;
                    break;
                }
            }
            lck.unlock();
        }
        catch (exception &e)
        {
            break;
        }
    }
    //正常结束游戏、将消息处理移交给处理进程
    if (end)
    {
        for (auto &v : info)
        {
            v.second->Ready = false;
        }
        thread T(&Room_Process::room_process, this);
        T.detach();
    }
}
