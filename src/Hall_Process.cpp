#include "Hall_Process.h"
#include "Tank_Server.h"
using namespace std;

#define NumOfHallThread 4

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
            int send_sock = v->tcp_fd;
            if (send_sock == sock_accept)
                continue;
            try
            {
                Send_TCP(send_sock, Res);
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
        if (v->tcp_fd == sock_accept || v->states != hall)
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

    Send_TCP(sock_accept, Res);
}

void Create_Room(int sock_accept)
{
    Room_Process *newroom = new Room_Process(sock_accept);
    room_counter++;
    newroom->room_id = room_counter;
    room_list.emplace_back(newroom);

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
        if (v->room_id == id) // 房间可加入
        {
            if (v->user_count < v->user_limited)
            {
                Res.set_result(1);
                Send_TCP(sock_accept, Res);
                v->Add_player(sock_accept);
            }
            else // 房间人数已满
            {
                Res.set_result(0);
                Send_TCP(sock_accept, Res);
            }
        }
        else // 房间不存在/已解散
        {
            Res.set_result(-1);
            Send_TCP(sock_accept, Res);
        }
    }
}

void set_user_id(int &socket, Header &header, char *content)
{
    Message::Set_User_id Req;
    Req.ParseFromArray(content, header.length);
    for (auto &v : user_list)
    {
        if (v->tcp_fd == socket)
        {
            v->userid = Req.name();
        }
    }
}

queue<Hall_Recvinfo *> Hall_recvQueue;  // 接收队列
mutex Hall_Recvinfo_mtx;                // 接收队列的锁
mutex Hall_Recvprocess_mtx;             // 唤醒处理线程的锁
condition_variable Hall_Recvprocess_cv; // 唤醒处理线程的条件变量
bool Hall_Process_stop = false;

void Hall_Process()
{
    Hall_Recvinfo *pRecvinfo = nullptr;
    while (!Hall_Process_stop)
    {
        unique_lock<mutex> lck(Hall_Recvprocess_mtx);
        Hall_Recvprocess_cv.wait(lck);
        lck.release()->unlock();

        unique_lock<mutex> qlck(Hall_Recvinfo_mtx);
        if (!Hall_recvQueue.empty())
        {
            pRecvinfo = Hall_recvQueue.front();
            Hall_recvQueue.pop();
        }
        qlck.release()->unlock();

        if (!pRecvinfo)
            continue;

        Header &header = pRecvinfo->header;
        int socket = pRecvinfo->socket;
        char *content = pRecvinfo->content;

        switch (header.type)
        {
        case 800:
        {
            for (auto v : user_list)
            {
                if (v->tcp_fd == socket)
                    v->recv_udp_info(header,content);
            }
            break;
        }
        case 101:
        {
            Get_hall_info(socket);
            break;
        }
        case 103:
        {
            Hall_Message(socket, header, content);
            break;
        }
        case 104:
        {
            Create_Room(socket);
            break;
        }
        case 105:
        {
            Enter_Room(socket, header, content);
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
            send(socket, buf, sizeof(Header) + header.length, 0);
            break;
        }
        case 107:
        {
            set_user_id(socket, header, content);
            break;
        }
        }
        delete pRecvinfo;
        pRecvinfo = nullptr;
    }
}

void server_hall()
{
    Hall_Process_stop = false;

    thread thread_arr[NumOfHallThread]; // 申请多个处理线程(线程池)，处理大厅信息
    for (int i = 0; i < NumOfHallThread; i++)
    {
        thread_arr[i] = thread(Hall_Process);
    }

    char buffer[100];
    memset(buffer, '\0', 1024);
    bool stop = false;
    Hall_Recvinfo *recvinfo = nullptr;
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
            if ((socket == hall_pipe[0]) && (hall_events->events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(hall_pipe[0], signals, 1023, 0);
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
                            break;
                        }
                        }
                    }
                }
            }
            else if (hall_events->events & EPOLLRDHUP)
            {
                delfd(hall_epoll, socket);
                for (auto it = user_list.begin(); it != user_list.end(); it++)
                {
                    if ((*it)->tcp_fd == socket)
                    {
                        user_list.erase(it);
                        break;
                    }
                }
                close(socket);
            }
            else if (hall_events->events & EPOLLIN)
            {
                int recv_length = 0;
                int re_num = recv(socket, buffer, sizeof(Header), 0);
                while (re_num > 0)
                {
                    int count = 0;

                    recvinfo = new Hall_Recvinfo(socket);
                    // 获取头
                    recv_length += sizeof(Header);
                    memcpy(&recvinfo->header, buffer, recv_length);

                    // 获取内容（可能为空）
                    if (recvinfo->header.length > 0)
                    {
                        recvinfo->content = new char[recvinfo->header.length];
                        re_num = recv(socket, recvinfo->content, recvinfo->header.length, 0);
                    }

                    unique_lock<mutex> qlck(Hall_Recvinfo_mtx);
                    Hall_recvQueue.emplace(recvinfo); // 投递消息
                    qlck.release()->unlock();
                    Hall_Recvprocess_cv.notify_one(); // 唤醒其中一个处理线程

                    if (count == 5) // 计数，防止持续占用
                        break;

                    recv_length = 0;
                    re_num = recv(socket, buffer, sizeof(Header), 0);
                }
                if (re_num == 0)
                {
                    for (auto it = user_list.begin(); it != user_list.end(); it++)
                    {
                        if ((*it)->tcp_fd == socket)
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
    Hall_Process_stop = true;
    Hall_Recvprocess_cv.notify_all();
    for (int i = 0; i < NumOfHallThread; i++)
    {
        thread_arr[i].join();
    }
    close(hall_epoll);
}