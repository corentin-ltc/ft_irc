#pragma once

#define SEND_FLAGS 0

#define ENDL "\r\n"
#define PONG(token) "PONG :ft_irc " + token
#define RPL_WELCOME(client, nick) ":" + client + " 001 :Welcome to the ft_irc Network, " + nick
#define RPL_NICK(old_nick, new_nick) ":" + old_nick + " NICK " + new_nick

// errors
#define ERROR(reason) "ERROR :" + reason // might change to a variable
#define ERR_NOTREGISTERED ": 451 : You have not registered"
#define ERR_NEEDMOREPARAMS(command) ": 461 " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTERED ": 462 :Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH ": 464 :Password incorrect"
#define ERR_NONICKNAMEGIVEN(old_nick) ": 431 " + old_nick + " :No nickname given"
#define ERR_ERRONEUSENICKNAME(client, new_nick) ": 433 " + client + " " + new_nick + ":Erroneus nickname"

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
