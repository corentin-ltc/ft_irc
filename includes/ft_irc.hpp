#pragma once

#define SEND_FLAGS 0

#define ENDL "\r\n"
#define RPL_WELCOME(nick) ":ft_irc 001 " + nick + ":Welcome to the network, " + nick + ENDL

#include <csignal>
#include <cstring>
#include <fcntl.h> // fcntl()
#include <iostream>
#include <netinet/in.h> // struct sockaddr_in
#include <signal.h>
#include <string>
#include <sys/poll.h> // struct pollfd
#include <sys/poll.h>
#include <sys/poll.h> // struct pollfd
#include <vector>

#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color

void stopServer(int signal);
unsigned short parse_port(std::string string);
std::string parse_password(std::string string);
std::vector<std::string> split(std::string source, char delimiter);

typedef struct pollfd t_pollfd;
