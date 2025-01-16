#include "Server.hpp"

inline static bool find(std::string str, std::string needle)
{
	return (str.find(needle) == 0);
}

inline static void goto_next_word(std::string &str)
{
	size_t next_word = str.find_first_of(' ');
	if (next_word == std::string::npos)
		str.clear();
	else
		str.erase(0, next_word + 1);
}

void Server::handleCommand(Client &client, std::string cmd)
{
	if (find(cmd, "CAP"))
		return; // ignores CAP
	if (find(cmd, "PASS"))
		return (pass(client, cmd));
	if (find(cmd, "PING"))
		return (ping(client.getSocket(), cmd));
	// NOTE: cmds needing password
	if (client.isRegistered() == false)
		return (this->error(client.getSocket(), ERR_NOTREGISTERED));
	if (find(cmd, "NICK"))
		return (nick(client, cmd));
	if (find(cmd, "USER"))
		return;
	// NOTE: cmds needing full auth (nickname..)
	if (client.isCommandReady() == false)
		return (this->error(client.getSocket(), ERR_NOTREGISTERED));
	if (cmd.find("JOIN"))
		;
	if (cmd.find("LEAVE"))
		;
}

void Server::ping(int client_socket, std::string cmd)
{
	goto_next_word(cmd);
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
	goto_next_word(cmd);
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

	goto_next_word(cmd);
	// check for errors

	client.setNickname(cmd);
	this->sendToSocket(client.getSocket(), RPL_NICK(old_nick, cmd));
}
