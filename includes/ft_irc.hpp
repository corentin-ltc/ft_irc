#pragma once

#include <csignal>
#include <cstring>
#include <fcntl.h> // fcntl()
#include <iostream>
#include <netinet/in.h> // struct sockaddr_in
#include <string>
#include <sys/poll.h> // struct pollfd
#include <sys/poll.h>
#include <sys/socket.h> // socket()
#include <unistd.h>		// read
#include <vector>

#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color

typedef struct pollfd t_pollfd;
