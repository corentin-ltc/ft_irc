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

/* TODO:
 - (PRIVMSG target <text to send>)
 - Check that there is at least one target (ERR_NORECIPIENT)
 - Check that there is a text to send (ERR_NOTEXTTOSEND)
 - Handle multiple targets (PRIVMSG target1,target2,#target3 <text>)
 - Check if the target is a client or a channel
 - If it's a client, check if it exists (ERR_NOSUCHNICK)
 - If it's a channel, check that the client is in it (ERR_CANNOTSENDTOCHAN)k
 - Send to every target with either sendToSocket or sendToChannel, no server-side needed
 */
void Server::privmsg(Client *client, std::string cmd)
{
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
	channel->sendToChannel(RPL_PART(client->getClientString(), channel->getName()));
	disconnectClientFromChannel(client, channel);
}

/* TODO:
 * Check args count (2 mini) (ERR_NEEDMOREPARAMS) (OPER name password)
 */
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

/* TODO:
 * Check param numbers (2 mini) (ERR_NEEDMOREPARAMS)
 * Check if the channel exists (ERR_NOSUCHCHANNEL)
 * Check if client is in the channel (ERR_NOTONCHANNEL)
 * Check if client is a channel or global operator (ERR_CHANOPRIVISNEEDED)
 * Check if target is in the channel (ERR_USERNOTINCHANNEL)
 * Check if a reason was given (everything after the 2nd argument), if not use a default reason
 * Use sendToChannel(RPL_KICK) to perform the client-side kick
 * Use disconnectClientFromChannel() to perform the server-side kick
 */
void Server::kick(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel = goto_next_word(cmd);
	std::string target = goto_next_word(cmd);
	for (int i = 4; i <= clients.size() + 3; i++)
		sendToSocket(i, ":" + name + " KICK " + channel + " " + target + " " + cmd);
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
