#pragma once

#include "header.h"

struct Hall_Recvinfo
{
    int socket = 0;
    Header header;
    char *content = nullptr;
    Hall_Recvinfo(int socket) : socket(socket), content(nullptr) {}
    ~Hall_Recvinfo()
    {
        if (content)
        {
            delete content;
        }
    }
};

void Hall_Message(int socket, Header &header, char *content);

void Get_hall_info(int socket);

void Create_Room(int socket);

void Enter_Room(int socket, Header &header, char *content);

void set_user_id(int &socket, Header &header, char *content);

void Hall_Process();

void server_hall();