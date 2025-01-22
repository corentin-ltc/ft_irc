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
	else if (checkForbiddenChars(cmd, "#&:", " ", ""))
		this->sendToSocket(client->getSocket(), ERR_ERRONEUSENICKNAME(old_nick, cmd));
	else if (findClient(cmd))
		this->sendToSocket(client->getSocket(), ERR_NICKNAMEINUSE(client->getClientString(), cmd));
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
		if (args[0].size() > USERLEN)
			args[0].erase(args[0].begin() + USERLEN, args[0].end());
		client->setUsername(args[0]);
		client->setCommandReady();
	}
}

/* TODO:
 * Handle multiple arguments (JOIN chan1,chan2,chan3)
 * Transform the name to either lower or uppercase, "{}|^" to upper are "[]\~"
 * Check if name is valid ((start with '&', '#', '+' or '!') and no ' ', '^G', ',' inside and 50chars max)
 * Check if mode (+k) password. client syntax : (JOIN chan1 password,chan2 password)
 * Check if mode (+i) invite and if client was invited
 * Check if mode (+l) client limit and if reached
 * can handle "JOIN 0" if you want (makes you leave all channels)
 * add as channel operator on creation
 */
void Server::join(Client *client, std::string cmd)
{
	std::string channel_name = goto_next_word(cmd);

	if (channel_name.empty())
		return (sendToSocket(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("JOIN"))));
	Channel *channel = findChannel(channel_name);
	if (channel == NULL)
	{
		channels.push_back(new Channel(channel_name));
		channel = channels[channels.size() - 1];
	}
	client->addChannel(channel);
	channel->addUser(client);
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
	channel->sendToChannel(RPL_KICK(client->getClientString(), channel_string, target_string, reason));
	disconnectClientFromChannel(channel->findUser(target_string), channel);
}

/* TODO:
 - Check args count (2 mini) (ERR_NEEDMOREPARAMS) (INVITE nickname #chan)
 - Check that the channel exists (ERR_NOSUCHCHANNEL)
 - Check that the client is in the channel (ERR_NOTONCHANNEL)
 - Check if mode (+i) is on and if client is channel operator (ERR_CHANOPRIVISNEEDED)
 - Check if the target is already on the channel (ERR_USERONCHANNEL)
 - send to client RPL_INVITING and an INVITE message to the target to perform client-side invite
 - Add the target to channel->invite_list for server-side invite
 */
void Server::invite(Client *client, std::string cmd)
{
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
	std::string channel = goto_next_word(cmd);
	for (int i = 4; i <= clients.size() + 3; i++)
		sendToSocket(i, ":" + name + " TOPIC " + channel + " " + cmd);
}

void Server::mode(Client *client, std::string cmd)
{
}

// on critical error, send ERROR and disconnect
void Server::error(Client *client, std::string reason)
{
	this->sendToSocket(client->getSocket(), ERROR(reason));
	this->disconnectClient(client);
}
