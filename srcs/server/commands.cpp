#include "Server.hpp"
#include "ft_irc.hpp"

void Server::handleCommand(Client *client, std::string cmd)
{
	std::string cmd_name = goto_next_word(cmd);
	if (cmd_name == "INFO" || cmd_name == "admin")
		return (printInfos());
	if (cmd_name == "CAP")
		return; // ignores CAP
	if (cmd_name == "PASS")
		return (pass(client, cmd));
	if (cmd_name == "PING")
		return (ping(client->getSocket(), cmd));
	// NOTE: cmds needing password
	if (client->isRegistered() == false)
		return (sendToSocket(client->getSocket(), ERR_NOTREGISTERED));
	if (cmd_name == "NICK")
		return (nick(client, cmd));
	if (cmd_name == "USER")
		return (user(client, cmd));
	// NOTE: cmds needing full auth (nickname..)
	if (client->isCommandReady() == false)
		return (sendToSocket(client->getSocket(), ERR_NOTREGISTERED));
	if (cmd_name == "JOIN")
		return (join(client, cmd));
	if (cmd_name == "MODE")
		return (mode(client, cmd));
	if (cmd_name == "PRIVMSG")
		return (privmsg(client, cmd));
	if (cmd_name == "PART")
		return (part(client, cmd));
	handleOperatorCommand(client, cmd, cmd_name);
	this->sendToSocket(client->getSocket(), ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_name));
}
void Server::handleOperatorCommand(Client *client, std::string cmd, std::string cmd_name)
{
	if (cmd_name == "OPER")
		return (oper(client, cmd));
	// NOTE : Verify global operator before use
	// if (client->isGlobalOperator() == false)
	// 	sendToSocket(client->getSocket(), ERR_NOPRIVILEGES(client->getUsername()));
	if (cmd_name == "KICK")
		return (kick(client, cmd));
	if (cmd_name == "INVITE")
		return (invite(client, cmd));
	if (cmd_name == "TOPIC")
		return (topic(client, cmd));
}

void Server::pass(Client *client, std::string cmd)
{
	if (client->isRegistered())
		this->sendToSocket(client->getSocket(), ERR_ALREADYREGISTERED);
	else if (this->password != cmd)
		this->sendToSocket(client->getSocket(), ERR_PASSWDMISMATCH);
	else
		client->_register();
}

void Server::ping(int client_socket, std::string cmd)
{
	if (cmd.empty())
		this->sendToSocket(client_socket, ERR_NEEDMOREPARAMS(std::string("PONG")));
	else
		this->sendToSocket(client_socket, PONG(cmd));
}

void Server::nick(Client *client, std::string cmd)
{

	std::string old_nick = client->getNickname();

	if (cmd.empty())
		this->sendToSocket(client->getSocket(), ERR_NONICKNAMEGIVEN(old_nick));
	// TODO: Check format (no leading ":" or "#")
	else if (cmd.find_first_of(' ') != std::string::npos)
		this->sendToSocket(client->getSocket(), ERR_ERRONEUSENICKNAME(old_nick, cmd));
	// TODO: Check duplicate (ERR_NICKNAMEINUSE)
	else
	{
		client->setNickname(cmd);
		this->sendToSocket(client->getSocket(), RPL_NICK(old_nick, cmd));
		client->setCommandReady();
	}
}

void Server::user(Client *client, std::string cmd)
{
	std::vector<std::string> args = get_args(cmd);

	if (client->isCommandReady())
		this->sendToSocket(client->getSocket(), ERR_ALREADYREGISTERED);
	else if (args.size() < 4)
		this->sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("USER")));
	else
	{
		client->setUsername(args[0]);
		client->setCommandReady();
		// TODO: set realname to a concatenation of args[3] to args[size]
	}
}

void Server::join(Client *client, std::string cmd)
{
	// TODO: handle multiple arguments
	std::string channel_name = goto_next_word(cmd);

	if (channel_name.empty())
		return (sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("JOIN"))));
	// TODO: Check that the channel name respects the norm
	Channel *channel = findChannel(channel_name);
	if (channel == NULL)
	{
		channels.push_back(new Channel(channel_name));
		channel = channels[channels.size() - 1];
	}
	if (channel->isInvitationMode())
	{
		if (channel->findUserInInvitationList(client))
		{
			client->addChannel(channel);
			channel->addUser(client);
		}
		else
		{
			sendToSocket(client->getSocket(), "Sorry you were not invited to this channel.");
			return ;
		}
	}
	else
		client->addChannel(channel);
		channel->addUser(client);
}

void Server::privmsg(Client *client, std::string cmd)
{
	// TODO: handle multiple arguments
	// TODO: send to user or channel
	Channel *channel = findChannel(goto_next_word(cmd));
	// TODO: send to everyone except user
	channel->sendToChannel(":" + client->getClientString() + " PRIVMSG " + channel->getName() + " " + cmd, client);
}
void Server::part(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel_name = goto_next_word(cmd);
	Channel *channel = findChannel(channel_name);
	// TODO: Check if the channel exist
	if (channel == NULL)
		return (sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(name, channel_name)));
	// TODO: Check if the user is inside the channel
	if (channel->findUser(client) == NULL)
		return (sendToSocket(client->getSocket(), ERR_NOTONCHANNEL(name, channel_name)));
	disconnectClientFromChannel(client, channel);
}

void Server::oper(Client *client, std::string cmd)
{
	std::string name = goto_next_word(cmd);
	std::string pass = cmd;
	if (name != NAME_ADMIN || pass != PASS_ADMIN)
		return (this->sendToSocket(client->getSocket(), ERR_PASSWDMISMATCH));
	else
		this->sendToSocket(client->getSocket(), RPL_YOUREOPER(client->getNickname()));
	client->setGlobalOperator();
}
void Server::kick(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel = goto_next_word(cmd);
	std::string target = goto_next_word(cmd);
	// TODO : Permission verification
	// TODO : Error verification
	// TODO : Delete users in channel as well
	// TODO: use channel->sendToChannel(RPL_KICK);
	for (int i = 4; i <= clients.size() + 3; i++)
		sendToSocket(i, ":" + name + " KICK " + channel + " " + target + " " + cmd);
}

void Server::invite(Client *client, std::string cmd)
{
}

void Server::topic(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel = goto_next_word(cmd);
	for (int i = 4; i <= clients.size() + 3; i++)
		sendToSocket(i, ":" + name + " TOPIC " + channel + " " + cmd);
}

void Server::mode(Client *client, std::string cmd)
{
	int flag_index = cmd.find_first_of(" ");
	std::string flag = cmd.substr(flag_index + 1, cmd.find_last_of(" "));
	if (cmd[0] == '#') // set mode for a channel
	{
		Channel *channel = findChannel(cmd.substr(0, flag_index));
		if (!channel)
			return (sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickname(), cmd.substr(0, flag_index))));
		if (flag == "+i")
			channel->setInvitationMode(true);
		else if (flag == "-i")
			channel->setInvitationMode(false);
		else if (flag == "-k")
			channel->setPasswordRequired(false);
		else if (flag == "+k")
		{
			std::string new_password = cmd.substr(flag_index + 2, cmd.size());
			if (new_password.size() == 0)
				return;// not enough arguments
			channel->setPasswordRequired(true);
			channel->setPassword(new_password);
		}
		else if (flag == "-o")
		{
			
		}
		else if (flag == "+o")
		{

		}
		else if (flag == "-l")
			channel->setUserLimit(__INT_MAX__);
		else if (flag == "+l")
		{

			std::string limit_str = cmd.substr(flag_index + 2, cmd.size());
			if (limit_str.size() == 0)
				return (sendToSocket(client->getSocket(), "Not enough parameter for user limit."));
			for (std::string::iterator it = limit_str.begin(); it != limit_str.end(); it++)
			{
				if (!isdigit(*it))
					return (sendToSocket(client->getSocket(), "Wrong parameter for user limit."));
			}
			int limit = atoi(limit_str.c_str());
			if (limit <= 0)
					return (sendToSocket(client->getSocket(), "User limit can't be negative."));
			channel->setUserLimit(limit);
		}
	}
	else // set mode for a user
	{
		// find user
	}




	std::cout << "Flag = " << flag << std::endl;
}

// on critical error, send ERROR and disconnect
void Server::error(Client *client, std::string reason)
{
	this->sendToSocket(client->getSocket(), reason);
	this->disconnectClient(client);
}
