#include "header.h"

std::string LOCAL_IP;

void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void addfd(int epollfd, int fd, bool block)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    if (block)
        setnonblocking(fd);
}

void delfd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

int get_new_socket(std::string IP, uint16_t socket_port, __socket_type protocol, sockaddr_in &sock_addr)
{
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(socket_port);

    sock_addr.sin_addr.s_addr = inet_addr(IP.c_str());

    int socket_fd = socket(PF_INET, protocol, 0);

    int result = 0;
    result = bind(socket_fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr));
    if (result)
    {
        perror("bind socket error");
        return -1;
    }

    return socket_fd;
}