#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <thread>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <iostream>
#include <assert.h>
#include <unordered_map>

using namespace std;

enum states
{
    hall,
    room,
    gaming
};
struct sock_info
{
    int accept;
    sockaddr_in addr;
    sock_info(int acc, sockaddr_in add)
    {
        accept = acc;
        addr = add;
    }
    int states = hall;
};

struct room_info
{
    int room_id;
    int user1 = 0;
    int user2 = 0;
};

enum{UP,DOWN,LEFT,RIGHT};
class Tank
{
public:
	int locationX = 0;
	int locationY = 0;
	int width = 0;
	int height = 0;
	int direction = UP;

	bool isalive = true;
	bool isregister = false;
    // void *bullet_head=NULL;
};

void setnonblocking(int fd);

void addfd(int epollfd, int fd);

void delfd(int epollfd, int fd);

void sig_handler(int sig);

void addsig(int sig);

string get_IP(int socket);

void Send_Message(int sock_accept, string &send_str);

string Get_hall_user(int sock_accept, string &s);

void Get_hall_room(int sock_accept, string &s);

string return_class(int &sock_accept, string &s);

void return_game_class(int mysocket, int opsocket, char buf[]);

void game(int socket1, int socket2);
