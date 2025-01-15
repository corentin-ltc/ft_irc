#pragma once

#include <sys/poll.h>
#include <cstring>
#include <vector>
#include <netinet/in.h> // struct sockaddr_in
#include <string>
#include <sys/poll.h> // struct pollfd
#include <vector>
#include <iostream>
#include <signal.h>

#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color

typedef struct pollfd t_pollfd;
