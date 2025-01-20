#include "Server.hpp"
#include "ft_irc.hpp"

// helper function that returns the current word and erase it from the source string
inline static std::string goto_next_word(std::string &str)
{
	size_t next_word = str.find_first_of(' ');
	std::string current_word = str.substr(0, next_word);
	if (next_word == std::string::npos)
		str.clear();
	else
		str.erase(0, next_word + 1);
	return (current_word);
}

inline static std::vector<std::string> get_args(std::string &str)
{
	std::vector<std::string> args;

	while (str.find_first_of(' ') != std::string::npos)
		args.push_back(goto_next_word(str));
	args.push_back(str);
	return (args);
}

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
		return (this->error(client->getSocket(), ERR_NOTREGISTERED));
	if (cmd_name == "NICK")
		return (nick(client, cmd));
	if (cmd_name == "USER")
		return (user(client, cmd));
	// NOTE: cmds needing full auth (nickname..)
	if (client->isCommandReady() == false)
		return (this->error(client->getSocket(), ERR_NOTREGISTERED));
	if (cmd_name == "JOIN")
		return (join(client, cmd));
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
	// 	this->error(client->getSocket(), ERR_NOPRIVILEGES(client->getUsername()));
	if (cmd_name == "KICK")
		return (kick(client, cmd));
	if (cmd_name == "INVITE")
		return (invite(client, cmd));
	if (cmd_name == "TOPIC")
		return (topic(client, cmd));
	if (cmd_name == "MODE")
		return (mode(client, cmd));
}

void Server::privmsg(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel = goto_next_word(cmd);
	// TODO : Message to channel/users only instead of everyone
	for (int i = 4; i <= clients.size() + 3; i++)
		if (i != client->getSocket())
			sendToSocket(i, ":" + name + " PRIVMSG " + channel + " " + cmd);
}

void Server::kick(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel = goto_next_word(cmd);
	std::string target = goto_next_word(cmd);
	// TODO : Permission verification
	// TODO : Error verification
	// TODO : Delete users in channel as well
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
}

void Server::ping(int client_socket, std::string cmd)
{
	if (cmd.empty())
		this->sendToSocket(client_socket, ERR_NEEDMOREPARAMS(std::string("PONG")));
	else
		this->sendToSocket(client_socket, PONG(cmd));
}

void Server::error(int client_socket, std::string reason)
{
	this->sendToSocket(client_socket, reason);
	// this->disconnectClient(client_socket);
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

void Server::join(Client *client, std::string cmd)
{

	std::string channel_name = cmd.substr(0, cmd.find(' '));
	// TODO: Check that the server name respects the norm
	if (channel_name.size() == 0)
	{
		std::cout << client->getSocket() << "Usage: /join <channel_name>" << std::endl;
		return;
	}
	std::vector<Channel *>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->getName() == channel_name)
		{
			// Joining an existing channel
			(*it)->addUser(client);
			return;
		}
	}
	if (it == channels.end())
	{
		// Creating a new channel
		Channel *newChan = new Channel(channel_name);
		newChan->addUser(client);
		channels.push_back(newChan);
		client->addChannel(newChan);
	}
}

void Server::part(Client *client, std::string cmd)
{
	std::string name = client->getNickname();
	std::string channel_name = goto_next_word(cmd);
	Channel *channel = findChannel(channel_name);
	// TODO: Check if the channel exist
	if (channel == NULL)
		return; // sendToSocket(client->getSocket(), ERR_NOSUCHCHANNEL)
	// TODO: Check if the user is inside the channel
	if (channel->findUser(client) == NULL)
		return; // sendToSocket(client->getSocket(), ERR_NOTONCHANNEL);
				// TODO : Remove the user from the channel list
	channel->sendToChannel(RPL_PART(client->getClientString(), channel_name));
	channel->disconnectUser(client);
	// TODO : Send the Part message to those in the channel only
}
