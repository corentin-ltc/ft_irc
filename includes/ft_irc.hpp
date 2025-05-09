#pragma once

#define SEND_FLAGS									   0
#define SHOW_LOGS									   1

#define ENDL										   "\r\n"
#define PONG(token)									   "PONG :ft_irc " + token
#define RPL_WELCOME(client, nick)					   ":" + client + " 001 " + nick + " :Welcome to ft_irc, " + nick
#define RPL_NICK(old_nick, new_nick)				   ":" + old_nick + " NICK " + new_nick
#define RPL_YOUREOPER(client)						   client + " :You are now an IRC operator"
#define RPL_PART(client, channel, reason)			   ":" + client + " PART " + channel + " :" + reason
#define RPL_JOIN(client, channel)					   ":" + client + " JOIN " + channel
#define RPL_INVITE(client, nick, channel)			   ":" + client + " INVITE " + nick + " " + channel
#define RPL_PRIVMSG(client, target, message)		   ":" + client + " PRIVMSG " + target + " :" + message
#define RPL_UMODEIS(nick, modestring)				   ": 221 " + nick + " " + modestring
#define RPL_CHANNELMODEIS(client, channel, modestring) ": 324 " + client + " " + channel + " " + modestring
#define RPL_NOTOPIC(client, channel)				   ": 331 " + client + " " + channel + " :No topic is set"
#define RPL_TOPIC(client, channel, topic)			   ": 332 " + client + " " + channel + " :" + topic
#define RPL_INVITING(client, nick, channel)			   ": 341 " + client + " " + nick + " channel"
#define RPL_NAMREPLY(client, channel, users)		   ": 353 " + client + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(client, channel)				   ": 366 " + client + " " + channel + " :End of /NAMES list"
#define RPL_KICK(client, channel, target, reason)	   ":" + client + " KICK " + channel + " " + target + " :" + reason

// errors
#define ERR_NOSUCHNICK(client, nick)				   ": 401 " + client + " " + nick + " :No such nick/channel"
#define ERR_CANNOTSENDTOCHAN(client, channel)		   ": 404 " + client + " " + channel + " :Cannot send to channel"
#define ERR_NORECIPIENT(client, command)			   ": 411 " + client + " :No recipient given (" + command + ")"
#define ERR_NOTEXTTOSEND(client)					   ": 412 " + client + " :No text to send"
#define ERR_UNKNOWNCOMMAND(client, command)			   ": 421 " + client + " " + command + " :Unknown command"
#define ERROR(reason)								   "ERROR :" + reason
#define ERR_USERNOTINCHANNEL(client, nick, channel)	   ": 441 " + client + " " + nick + " " + channel + " :They aren't on that channel"
#define ERR_USERONCHANNEL(client, nick, channel)	   ": 443 " + client + " " + nick + " " + channel + " :is already on channel"
#define ERR_NOTREGISTERED							   ": 451 : You have not registered"
#define ERR_NEEDMOREPARAMS(command)					   ": 461 " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTERED						   ": 462 :Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH							   ": 464 :Password incorrect"
#define ERR_NONICKNAMEGIVEN(old_nick)				   ": 431 " + old_nick + " :No nickname given"
#define ERR_ERRONEUSENICKNAME(client, new_nick)		   ": 432 " + client + " " + new_nick + " :Erroneus nickname"
#define ERR_NICKNAMEINUSE(client, nick)				   ": 433 " + client + " " + nick + " :Nickname is already in use"
#define ERR_CHANNELISFULL(client, channel)			   ": 471 " + client + " " + channel + " :Cannot join channel(+l)"
#define ERR_INVITEONLYCHAN(client, channel)			   ": 473 " + client + " " + channel + " :Cannot join channel(+i)"
#define ERR_BADCHANNELKEY(client, channel)			   ": 475 " + client + " " + channel + " :Cannot join channel (+k)"
#define ERR_BADCHANMASK(channel)					   ": 476 " + channel + " :Bad Channel Mask"
#define ERR_NOPRIVILEGES(client)					   ": 481 :" + client + ":Permission Denied- You're not an IRC operator"
#define ERR_CHANOPRIVISNEEDED(client, channel)		   ": 482 " + client + " " + channel + " :You're not channel operator"
#define ERR_NOSUCHCHANNEL(client, channel)			   ":ft_irc 403 " + client + " " + channel + " :No such channel"
#define ERR_NOTONCHANNEL(client, channel)			   ":ft_irc 442 " + client + " " + channel + " :Not on channel"
#define ERR_UMODEUNKNOWNFLAG(client, mode)			   ": 501 " + client + " :Unknown MODE flag " + mode

#define NAME_ADMIN									   "admin"
#define PASS_ADMIN									   "admin"
#define USERLEN										   18

#include <csignal>
#include <cstring>
#include <fcntl.h>		// fcntl()
#include <iostream>
#include <netinet/in.h> // struct sockaddr_in
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/poll.h>  // struct pollfd
#include <sys/poll.h>
#include <sys/poll.h>  // struct pollfd
#include <vector>

#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color

void					 stopServer(int signal);
unsigned short			 parse_port(std::string string);
std::string				 parse_password(std::string string);
std::vector<std::string> split(std::string source, char delimiter);
std::string				 goto_next_word(std::string &str);
std::string				 goto_next_param(std::string &str);
std::vector<std::string> get_args(std::string &str);
bool					 checkForbiddenChars(std::string src, std::string leading_chars, std::string charset, std::string ending_chars);
std::string				 chanToLower(std::string channel_name);
bool					 checkChannelNameFormat(std::string channel_name);

typedef struct pollfd	 t_pollfd;

#
