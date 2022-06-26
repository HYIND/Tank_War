#pragma once

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
#include <queue>
#include <mutex> 
#include <condition_variable>
void setnonblocking(int fd);

void addfd(int epollfd, int fd);

void delfd(int epollfd, int fd);