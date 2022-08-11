#include "Room_Game_Process.h"
#include "collision.h"

void Room_Process::confim()
{
    // clock_t start, now, timeout;
    // timeout = 20 * CLOCKS_PER_SEC;

    // map<int, bool> confim;
    // string send_str = "ConfimMapid:" + to_string(map_id);
    // for (auto &v : info)
    // {
    //     confim[v.first] = false;
    //     send(v.first, (const char *)&(send_str[0]), 1023, 0);
    // }

    // socket_messageinfo *messageinfo = nullptr;
    // string str;
    // char *buf;
    // int cur_socket;
    // start = clock();
    // while (!recv_queue.empty())
    // {
    //     now = clock();
    //     if (now - start > timeout)
    //         break;
    //     unique_lock<mutex> qlck(recvqueue_mtx);
    //     messageinfo = recv_queue.front();
    //     recv_queue.pop();
    //     qlck.unlock();
    //     cur_socket = messageinfo->socket;
    //     buf = messageinfo->ch;
    //     str = messageinfo->ch;
    //     string::const_iterator iterStart = str.begin();
    //     string::const_iterator iterEnd = str.end();
    //     smatch m;
    //     regex reg("^[A-Z|a-z]+");
    //     regex_search(iterStart, iterEnd, m, reg);
    //     string option;
    //     option = m[0];
    //     if (option == "ConfimMapid")
    //     {
    //         try
    //         {
    //             string recv_id(m[0].second + 1, iterEnd);
    //             if (map_id == atoi(recv_id.c_str()))
    //             {
    //                 confim[cur_socket] = true;
    //             }
    //         }
    //         catch (exception &e)
    //         {
    //             continue;
    //         }
    //     }
    //     delete messageinfo;
    //     messageinfo = nullptr;
    // }
    // for (auto &v : confim)
    // {
    //     if (v.second == false)
    //     {
    //         info.erase(v.first);
    //         for (auto it = user_list.begin(); it != user_list.end(); it++)
    //         {
    //             if ((*it)->accept == v.first)
    //             {
    //                 user_list.erase(it);
    //                 break;
    //             }
    //         }
    //         user_count--;
    //     }
    // }
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
                tank->rotate = m.rotate;
                tank->isalive = m.isalive;
            }
        }
        Tank_info[v.first] = tank;
    }

    string send_str = "Start"; //开始游戏
    for (auto &v : info)
    {
        Message::Room_Start_Response Res;
        Res.set_result(1);
        Send(v.first, Res);
    }
}

void Room_Process::EndGame()
{
    for (auto &v : info)
    {
        if (Tank_info[v.first]->isalive)
        {
            socket_sendinfo *psendinfo = new socket_sendinfo(v.first, 227);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(psendinfo);
            sendqueue_mtx.unlock();
        }
        else
        {
            socket_sendinfo *psendinfo = new socket_sendinfo(v.first, 228);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(psendinfo);
            sendqueue_mtx.unlock();
        }
    }
}

bool Room_Process::Hit_tank(int socket, Header &header, char *content)
{
    Message::Game_hit_tank_Request Req;
    Req.ParseFromArray(content, header.length);

    const Message::bulletinfo *pbinfo = &(Req.bulletinfo());
    int hited_tank_id = Req.hited_tank_id();
    double bullet_x = pbinfo->locationx();
    double bullet_y = pbinfo->locationy();
    double bullet_rotate = pbinfo->rotate();
    BulletStyle style = (BulletStyle)pbinfo->bullet_style();

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
    if (collision_obb(bullet_x,
                      bullet_y,
                      Bullet_Style_info[style]->width,
                      Bullet_Style_info[style]->height,
                      bullet_rotate,
                      hited_tank->locationX,
                      hited_tank->locationY,
                      Tank_Style_info[hited_tank->tank_style]->width,
                      Tank_Style_info[hited_tank->tank_style]->height,
                      hited_tank->rotate))
    {
        hited_tank->health -= 21;
        hittank_notify(hited_socket, hited_tank_id, hited_tank->health);
        if (hited_tank->health <= 0)
        {
            hited_tank->isalive = false;
            destroy(hited_socket, hited_tank_id);
            return true;
        }
    }
    return false;
}

bool Room_Process::Hit_brick(int socket, Header &header, char *content)
{
    Message::Game_hit_brick_Request Req;
    Req.ParseFromArray(content, header.length);

    const Message::bulletinfo *info = &(Req.bulletinfo());
    int hited_brick_id = Req.hited_brick_id();
    double bullet_x = info->locationx();
    double bullet_y = info->locationy();
    double bullet_rotate = info->rotate();
    BulletStyle style = (BulletStyle)info->bullet_style();

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

    if (collision_obb(bullet_x,
                      bullet_y,
                      Bullet_Style_info[style]->width,
                      Bullet_Style_info[style]->height,
                      bullet_rotate,
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

void Room_Process::hittank_notify(int hited_socket, int hited_id, int health)
{
    socket_sendinfo *psendinfo = new socket_sendinfo(hited_socket, 224);
    unique_lock<mutex> slck(sendqueue_mtx);
    send_queue.emplace(psendinfo);
    sendqueue_mtx.unlock();

    Message::Game_tank_hited_Response Res;
    Res.set_hited_tank_id(hited_id);
    Res.set_health(health);

    for (auto &v : info)
    {
        if (v.first != hited_socket)
        {
            socket_sendinfo *pmsginfo = new socket_sendinfo(v.first, Res);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(pmsginfo);
            sendqueue_mtx.unlock();
        }
    }
}

void Room_Process::hitbrick_notify(int hited_brick_id, int health)
{
    Message::Game_brick_hited_Response Res;
    Res.set_hited_brick_id(hited_brick_id);
    Res.set_health(health);
    for (auto &v : info)
    {
        socket_sendinfo *psendinfo = new socket_sendinfo(v.first, Res);
        unique_lock<mutex> slck(sendqueue_mtx);
        send_queue.emplace(psendinfo);
        sendqueue_mtx.unlock();
    }
}

void Room_Process::destroy(int hited_socket, int hited_id)
{
    socket_sendinfo *psendinfo = new socket_sendinfo(hited_socket, 226);
    unique_lock<mutex> slck(sendqueue_mtx);
    send_queue.emplace(psendinfo);
    sendqueue_mtx.unlock();

    Message::Game_destroyed_tank_Response Res;
    Res.set_destroyed_tank_id(hited_id);

    for (auto &v : info)
    {
        if (v.first != hited_socket)
        {
            socket_sendinfo *psendinfo = new socket_sendinfo(v.first, Res);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(psendinfo);
            sendqueue_mtx.unlock();
        }
    }
}

void Room_Process::Refreash_tankinfo(int socket, Header &header, char *content)
{
    Message::Game_tankinfo_Request Req;
    Req.ParseFromArray(content, header.length);

    Tank *tank = Tank_info[socket];
    if (tank->isalive)
    {
        tank->locationX = Req.locationx();
        tank->locationY = Req.locationy();
        tank->rotate = Req.rotate();
        tank->tank_style = (TankStyle)Req.tank_style();
    }
}

void Room_Process::Refreash_bulletinfo(int socket, Header &header, char *content)
{
    Tank *tank = Tank_info[socket];
    if (!tank)
        return;
    Message::Game_bulletinfo_Request Req;
    Req.ParseFromArray(content, header.length);

    bullet *newhead = new bullet();
    bullet *temp = newhead;

    for (int i = 0; i < Req.bulletinfo_size(); i++)
    {
        const Message::bulletinfo info = Req.bulletinfo(i);
        temp->next = new bullet(info.locationx(), info.locationy(), info.rotate(), (BulletStyle)info.bullet_style());
        temp = temp->next;
    }
    tank->bullet_head = newhead->next;
    delete newhead;
}

void Room_Process::Refreash_Tank_process(bool *end)
{
    while (!stop && !(*end))
    {
        struct timeval temp;
        temp.tv_sec = 0;
        temp.tv_usec = 18000;
        select(0, NULL, NULL, NULL, &temp);
        Message::Game_tankinfo_Response tank_Res;
        for (auto &v : Tank_info)
        {
            Tank *tank = v.second;
            Message::Game_tankinfo_Response_tankinfo *Res_tankinfo = tank_Res.add_info();

            Res_tankinfo->set_id(info[v.first]->tank_id);
            Res_tankinfo->set_health(tank->health);
            Message::Game_tankinfo_Request *tankinfo = new Message::Game_tankinfo_Request();
            tankinfo->set_locationx(tank->locationX);
            tankinfo->set_locationy(tank->locationY);
            tankinfo->set_rotate(tank->rotate);
            tankinfo->set_tank_style((int)tank->tank_style);
            Res_tankinfo->set_allocated_tankinfo(tankinfo);
        }
        for (auto &v : info)
        {
            socket_sendinfo *psendinfo = new socket_sendinfo(v.first, tank_Res);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(psendinfo);
            sendqueue_mtx.unlock();
            send_cv.notify_one();
        }
    }
}

void Room_Process::Refreash_Bullet_process(bool *end)
{
    timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 9000;
    nanosleep(&delay, &delay);

    while (!stop && !(*end))
    {
        struct timeval temp;
        temp.tv_usec = 18000;
        select(0, NULL, NULL, NULL, &temp);
        Message::Game_bulletinfo_Response bullet_Res;
        for (auto &v : Tank_info)
        {
            Tank *tank = v.second;
            Message::Game_bulletinfo_Response_Info *Res_bulletinfo = bullet_Res.add_info();

            Res_bulletinfo->set_tankid(info[v.first]->tank_id);

            bullet *cur = tank->bullet_head;
            while (cur != NULL)
            {
                Message::bulletinfo *bulletinfo = Res_bulletinfo->add_bulletinfo();
                bulletinfo->set_locationx(cur->locationX);
                bulletinfo->set_locationy(cur->locationY);
                bulletinfo->set_rotate(cur->rotate);
                bulletinfo->set_bullet_style((int)cur->bullet_style);
                cur = cur->next;
            }
        }
        for (auto &v : info)
        {
            socket_sendinfo *psendinfo = new socket_sendinfo(v.first, bullet_Res);
            unique_lock<mutex> slck(sendqueue_mtx);
            send_queue.emplace(psendinfo);
            sendqueue_mtx.unlock();
            send_cv.notify_one();
        }
    }
}

int Room_Process::return_class_game(int socket, Header &header, char *content)
{
    switch (header.type)
    {
    case 106:
    {
        header.type = 206;
        char buf[sizeof(Header) + header.length] = {'\0'};
        memcpy(buf, &header, sizeof(Header));
        memcpy(buf + sizeof(Header), content, header.length);
        send(socket, buf, sizeof(Header) + header.length, 0);
        break;
    }
    case 120:
        Refreash_tankinfo(socket, header, content);
        break;
    case 121:
        Refreash_bulletinfo(socket, header, content);
        break;
    case 122:
        Hit_brick(socket, header, content);
        break;
    case 123:
    {
        if (Hit_tank(socket, header, content))
        {
            return 1;
        }
        break;
    }
    case 129:
        unique_lock<mutex> infolck(info_mtx);
        info[socket]->sockinfo->states = hall;
        delfd(recv_epoll, socket);
        addfd(hall_epoll, socket);
        info.erase(socket);
        infolck.unlock();
        break;
    }
    return 0;
}

void Room_Process::game_process()
{
    socket_messageinfo *messageinfo = NULL;
    string str;
    int cur_socket;

    // confim();
    // if (user_count == 0)
    // {
    //     stop = true;
    // }

    Init_Game();

    bool end = false;
    int player_alive = user_count;

    thread Tankinfo_thread(&Room_Process::Refreash_Tank_process, this, &end);
    thread Bulletinfo_thread(&Room_Process::Refreash_Bullet_process, this, &end);

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

                if (return_class_game(messageinfo->socket, messageinfo->header, messageinfo->content))
                    player_alive--;

                send_cv.notify_one();

                delete (messageinfo);
                messageinfo = NULL;
                if (player_alive == 1)
                {
                    EndGame();
                    end = true;
                    send_cv.notify_one();
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
    Tankinfo_thread.join();
    Bulletinfo_thread.join();
    //正常结束游戏、将消息处理移交给处理进程
    if (end)
    {
        for (auto &v : info)
        {
            v.second->Ready = false;
        }
        this->gaming = false;
        thread T(&Room_Process::room_process, this);
        T.detach();
    }
}
