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
	if (cmd_name == "PRIVMSG")
		return (privmsg(client, cmd));
	if (cmd_name == "PART")
		return (part(client, cmd));
	if (cmd_name == "OPER")
		return (oper(client, cmd));
	// NOTE: cmds needing op
	if (cmd_name == "KICK")
		return (kick(client, cmd));
	if (cmd_name == "INVITE")
		return (invite(client, cmd));
	if (cmd_name == "TOPIC")
		return (topic(client, cmd));
	if (cmd_name == "MODE")
		return (mode(client, cmd));
	this->sendToSocket(client->getSocket(), ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_name));
}

void Server::pass(Client *client, std::string cmd)
{
	if (client->isRegistered())
		this->sendToSocket(client->getSocket(), ERR_ALREADYREGISTERED);
	else if (cmd.empty())
		this->sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("PASS")));
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
	else if (cmd.size() > 9 || checkForbiddenChars(cmd, "#&:", " ", ""))
		this->sendToSocket(client->getSocket(), ERR_ERRONEUSENICKNAME(old_nick, cmd));
	else if (findClient(cmd))
		this->sendToSocket(client->getSocket(), ERR_NICKNAMEINUSE(client->getClientString(), cmd));
	else
	{
		client->setNickname(cmd);
		for (size_t i = 0; i < clients.size(); i++)
			this->sendToSocket(clients[i]->getSocket(), RPL_NICK(old_nick, cmd));
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
		if (args[0].size() > USERLEN)
			args[0].erase(args[0].begin() + USERLEN, args[0].end());
		client->setUsername(args[0]);
		client->setCommandReady();
	}
}

void Server::join(Client *client, std::string cmd)
{
	std::string channels_string = goto_next_word(cmd);
	std::string keys_string = goto_next_word(cmd);
	std::vector<std::string> channel_names;
	std::vector<std::string> keys;

	if (channels_string.empty())
		return (sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("JOIN"))));
	channel_names = split(channels_string, ',');
	keys = split(keys_string, ',');
	for (size_t i = 0; i < channel_names.size(); i++)
	{
		if (checkChannelNameFormat(channel_names[i]) == false)
		{
			if (channel_names[i] == "0")
				disconnectClientFromAllChannels(client, "No reason given");
			else
				sendToSocket(client->getSocket(), ERR_BADCHANMASK(channel_names[i]));
			continue;
		}
		std::string channel_name = chanToLower(channel_names[i]);
		Channel *channel = findChannel(channel_name);
		if (channel == NULL) // create new channel and set operator
		{
			channels.push_back(new Channel(channel_name));
			channel = channels[channels.size() - 1];
			channel->getOperators().push_back(client);
		}
		else // channel already exists, check modes
		{
			if (channel->isPasswordRequired() == true && (i >= keys.size() || keys[i] != channel->getPassword()))
			{
				sendToSocket(client->getSocket(), ERR_BADCHANNELKEY(client->getClientString(), channel_names[i]));
				continue;
			}
			if (channel->isInvitationMode() == true && channel->findInvite(client->getNickname()) == false)
			{
				sendToSocket(client->getSocket(), ERR_INVITEONLYCHAN(client->getClientString(), channel_names[i]));
				continue;
			}
			if (channel->getUsers().size() >= channel->getMaxUsers())
			{
				sendToSocket(client->getSocket(), ERR_CHANNELISFULL(client->getClientString(), channel_names[i]));
				continue;
			}
		}
		client->addChannel(channel);
		channel->addUser(client);
	}
}

void Server::privmsg(Client *client, std::string cmd)
{
	std::string targets_string = goto_next_word(cmd);
	if (targets_string.empty())
		return (this->sendToSocket(client->getSocket(), ERR_NORECIPIENT(client->getClientString(), "PRIVMSG")));
	std::string text = cmd;
	if (text.empty())
		return (this->sendToSocket(client->getSocket(), ERR_NOTEXTTOSEND(client->getClientString())));
	if (text[0] == ':')
		text.erase(text.begin());
	std::vector<std::string> targets = split(targets_string, ',');
	for (size_t i = 0; i < targets.size(); i++)
	{
		if (std::string("&#+!").find(targets[i][0]) != std::string::npos) // target is a channel
		{
			Channel *channel_target = findChannel(targets[i]);
			if (channel_target == NULL) // chan does not exist
				sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(client->getClientString(), targets[i]));
			else if (channel_target->findUser(client) == NULL) // client not in chan
				sendToSocket(client->getSocket(), ERR_CANNOTSENDTOCHAN(client->getClientString(), targets[i]));
			else // send reply
				channel_target->sendToChannel(RPL_PRIVMSG(client->getClientString(), targets[i], text), client);
		}
		else // target is a client
		{
			Client *client_target = findClient(targets[i]);
			if (client_target == NULL)
				sendToSocket(client->getSocket(), ERR_NOSUCHNICK(client->getClientString(), targets[i]));
			else
				sendToSocket(client_target->getSocket(), RPL_PRIVMSG(client->getClientString(), targets[i], text));
		}
	}
}

void Server::part(Client *client, std::string cmd)
{
	std::string channel_name = goto_next_word(cmd);
	std::string reason = cmd.empty() ? "No reason given" : cmd;

	if (channel_name.empty())
		return (this->sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("PART"))));
	Channel *channel = findChannel(channel_name);
	if (channel == NULL)
		return (sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(client->getClientString(), channel_name)));
	if (channel->findUser(client) == NULL)
		return (sendToSocket(client->getSocket(), ERR_NOTONCHANNEL(client->getClientString(), channel_name)));
	if (reason.at(0) == ':')
		reason.erase(reason.begin());
	channel->sendToChannel(RPL_PART(client->getClientString(), channel->getName(), reason));
	disconnectClientFromChannel(client, channel);
}

void Server::oper(Client *client, std::string cmd)
{
	std::string name = goto_next_word(cmd);
	std::string pass = cmd;

	if (name.empty() || pass.empty())
		return (this->sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("OPER"))));
	if (name != NAME_ADMIN || pass != PASS_ADMIN)
		return (this->sendToSocket(client->getSocket(), ERR_PASSWDMISMATCH));
	this->sendToSocket(client->getSocket(), RPL_YOUREOPER(client->getNickname()));
	client->setGlobalOperator();
}

void Server::kick(Client *client, std::string cmd)
{
	std::string channel_string = goto_next_word(cmd);
	std::string target_string = goto_next_word(cmd);
	std::string reason = cmd.empty() ? "No reason given" : cmd;

	if (channel_string.empty() || target_string.empty())
		return (this->sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("KICK"))));
	Channel *channel = findChannel(channel_string);
	if (channel == NULL)
		return (this->sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(client->getClientString(), channel_string)));
	if (channel->findUser(client) == NULL)
		return (this->sendToSocket(client->getSocket(), ERR_NOTONCHANNEL(client->getClientString(), channel_string)));
	if (client->isGlobalOperator() == false && channel->isOperator(client) == false)
		return (this->sendToSocket(client->getSocket(), ERR_CHANOPRIVISNEEDED(client->getClientString(), channel_string)));
	Client *target = channel->findUser(target_string);
	if (target == NULL)
		return (this->sendToSocket(client->getSocket(), ERR_USERNOTINCHANNEL(client->getClientString(), target_string, channel_string)));
	if (reason.at(0) == ':')
		reason.erase(reason.begin());
	channel->sendToChannel(RPL_KICK(client->getClientString(), channel_string, target_string, reason));
	disconnectClientFromChannel(channel->findUser(target_string), channel);
}

void Server::invite(Client *client, std::string cmd)
{
	std::string target_name = goto_next_word(cmd);
	std::string channel_name = goto_next_word(cmd);

	if (target_name.empty() || channel_name.empty())
		return (sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("INVITE"))));
	Channel *channel = findChannel(channel_name);
	if (channel == NULL)
		return (sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(client->getClientString(), channel_name)));
	if (channel->findUser(client) == NULL)
		return (sendToSocket(client->getSocket(), ERR_NOTONCHANNEL(client->getClientString(), channel_name)));
	// if (channel->getInviteMode() == true && (client->isGlobalOperator() == false && channel->isOperator(client) == false))
	// 	return (sendToSocket(client->getSocket(), ERR_CHANOPRIVISNEEDED(client->getClientString(), channel_name)));
	Client *target = findClient(target_name);
	if (target != NULL && channel->findUser(target))
		return (sendToSocket(client->getSocket(), ERR_USERONCHANNEL(client->getClientString(), target_name, channel_name)));
	sendToSocket(client->getSocket(), RPL_INVITING(client->getClientString(), target_name, channel_name));
	if (target != NULL)
		sendToSocket(target->getSocket(), RPL_INVITE(client->getClientString(), target_name, channel_name));
	if (channel->findInvite(target_name) == false)
		channel->addInvite(target_name);
}

/*TODO:
 - Check args count (1 mini) (ERR_NEEDMOREPARAMS) (TOPIC #chan)
 - Check if the channel exists (ERR_NOSUCHCHANNEL)
 - Check if the client is in the channel (ERR_NOTONCHANNEL)
 - If only 1 argument, just RPL_TOPIC or RPL_NOTOPIC and leave
 - Else, consider everything after ":" as the new topic, if empty topic must be cleared  (TOPIC #chan :new topic)
 - Check if mode (+t) is on, if yes check if client is allowed (ERR_CHANOPRIVISNEEDED)
 - SendToChannel() RPL_TOPIC to update on client-side
 - Channel->setTopic() to update on server-side
 */
void Server::topic(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel_string = goto_next_word(cmd);

	if (channel_string.empty())
		return (this->sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("TOPIC"))));
	Channel *channel = findChannel(channel_string);
	if (channel == NULL)
		return (this->sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL(client->getClientString(), channel_string)));
	if (channel->findUser(client) == NULL)
		return (this->sendToSocket(client->getSocket(), ERR_NOTONCHANNEL(client->getClientString(), channel_string)));
	if (client->isGlobalOperator() == false && channel->isOperator(client) == false)
		return (this->sendToSocket(client->getSocket(), ERR_CHANOPRIVISNEEDED(client->getClientString(), channel_string)));
	channel->sendToChannel(":" + name + " TOPIC " + channel_string + " " + cmd);
	channel->setTopic(client, cmd);
}

void Server::mode(Client *client, std::string cmd)
{
	int flag_index = cmd.find_first_of(" ");
	std::string flag = cmd.substr(flag_index + 1, 2);
	if (cmd[0] != '#') // set mode for users are not handled
		return;
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
		std::string new_password = cmd.substr(flag_index + 4);
		if (new_password.size() == 0)
			return;// not enough arguments
		channel->setPasswordRequired(true);
		channel->setPassword(new_password);
	}
	else if (flag == "-t")
	{
		
	}
	else if (flag == "+t")
	{
		
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
		std::string limit_str = cmd.substr(flag_index + 4);
		if (limit_str.size() == 0)
			return (sendToSocket(client->getSocket(), "Not enough parameter for user limit."));
		for (std::string::iterator it = limit_str.begin(); it != limit_str.end(); it++)
		{
			std::cout << limit_str << "\n";
			if (!isdigit(*it))
				return (sendToSocket(client->getSocket(), "Wrong parameter for user limit."));
		}
		int limit = atoi(limit_str.c_str());
		if (limit <= 0)
				return (sendToSocket(client->getSocket(), "User limit can't be negative."));
		channel->setUserLimit(limit);
	}
	
}

// on critical error, send ERROR and disconnect
// often used when not providing a password, here I chose to allow the user to be able to retry a PASS while staying connected
void Server::error(Client *client, std::string reason)
{
	this->sendToSocket(client->getSocket(), ERROR(reason));
	this->disconnectClient(client);
}
