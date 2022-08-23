#include "header.h"
#include "Tank_Server.h"
#include "Hall_Process.h"
#include "Map.h"
#include <net/if.h>
#include <sys/ioctl.h>
using namespace std;

#define DEFAULT_PORT 2336

int get_local_ip(const char *eth_inf, char *out)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    strcpy(out, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(sd);
    return 0;
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(DEFAULT_PORT);

    if (argc > 1)
    {
        sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    else
    {
        char IP[20] = {'\0'};
        get_local_ip("eth0", IP);
        sock_addr.sin_addr.s_addr = inet_addr(IP);
    }

    int mysocket = socket(PF_INET, SOCK_STREAM, 0);

    int result = 0;
    result = bind(mysocket, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr));
    if (result)
    {
        perror("bind socket error");
        return 0;
    }

    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, listen_pipe) != -1);
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, hall_pipe) != -1);
    setnonblocking(listen_pipe[1]);
    addfd(listen_epoll, listen_pipe[0]);
    setnonblocking(hall_pipe[1]);
    addfd(hall_epoll, hall_pipe[0]);
    // setnonblocking(game_pipe[1]);
    // addfd(game_epoll, game_pipe[0]);
    addsig(SIGINT);
    addsig(SIGTERM);
    signal(SIGPIPE, SIG_IGN);

    Init_Style();
    Init_Map();

    thread T1(server_listen, mysocket);

    thread T2(server_hall);

    T2.join();
    T1.join();
    close(listen_pipe[0]);
    close(listen_pipe[1]);
    close(hall_pipe[0]);
    close(hall_pipe[1]);
    return 0;
}
