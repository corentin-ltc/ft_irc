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

void Server::handleCommand(Client &client, std::string cmd)
{
	std::string cmd_name = goto_next_word(cmd);
	if (cmd_name == "CAP")
		return; // ignores CAP
	if (cmd_name == "PASS")
		return (pass(client, cmd));
	if (cmd_name == "PING")
		return (ping(client.getSocket(), cmd));
	// NOTE: cmds needing password
	if (client.isRegistered() == false)
		return (this->error(client.getSocket(), ERR_NOTREGISTERED));
	if (cmd_name == "NICK")
		return (nick(client, cmd));
	if (cmd_name == "USER")
		return (user(client, cmd));
	// NOTE: cmds needing full auth (nickname..)
	if (client.isCommandReady() == false)
		return (this->error(client.getSocket(), ERR_NOTREGISTERED));
	if (cmd_name == "JOIN")
		;
	if (cmd_name == "LEAVE")
		;
	this->sendToSocket(client.getSocket(), ERR_UNKNOWNCOMMAND(client.getNickname(), cmd_name));
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

void Server::pass(Client &client, std::string cmd)
{
	if (client.isRegistered())
		this->sendToSocket(client.getSocket(), ERR_ALREADYREGISTERED);
	else if (this->password != cmd)
		this->sendToSocket(client.getSocket(), ERR_PASSWDMISMATCH);
	else
		client._register();
}

void Server::nick(Client &client, std::string cmd)
{
	std::string old_nick = client.getNickname();

	if (cmd.empty())
		this->sendToSocket(client.getSocket(), ERR_NONICKNAMEGIVEN(old_nick));
	// TODO: Check format (no leading ":" or "#")
	else if (cmd.find_first_of(' ') != std::string::npos)
		this->sendToSocket(client.getSocket(), ERR_ERRONEUSENICKNAME(old_nick, cmd));
	// TODO: Check duplicate (ERR_NICKNAMEINUSE)
	else
	{
		client.setNickname(cmd);
		this->sendToSocket(client.getSocket(), RPL_NICK(old_nick, cmd));
		client.setCommandReady();
	}
}

void Server::user(Client &client, std::string cmd)
{
	std::vector<std::string> args = get_args(cmd);

	if (client.isCommandReady())
		this->sendToSocket(client.getSocket(), ERR_ALREADYREGISTERED);
	else if (args.size() < 4)
		this->sendToSocket(client.getSocket(), ERR_NEEDMOREPARAMS(std::string("USER")));
	else
	{
		client.setUsername(args[0]);
		client.setCommandReady();
		// TODO: set realname to a concatenation of args[3] to args[size]
	}
}
