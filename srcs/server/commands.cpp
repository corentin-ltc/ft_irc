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
	// NOTE: cmds without auth
	if (find(cmd, "CAP"))
		return; // ignores CAP
	if (find(cmd, "PASS"))
		return (pass(client, cmd));
	if (find(cmd, "USER"))
		return;
	if (find(cmd, "NICK"))
		return;
	if (find(cmd, "PING"))
		return (ping(client.getSocket(), cmd));
	if (client.isAuthentificated() == false)
		return; // Renvoyer une erreur client not registered/unknown command
	// NOTE: cmds needing authentification
	if (cmd.find("JOIN"))
		;
	if (cmd.find("LEAVE"))
		;
}

void Server::ping(int client_socket, std::string cmd)
{
	goto_next_word(cmd);
	this->sendToSocket(client_socket, PONG(cmd));
}

void Server::pass(Client &client, std::string cmd)
{
	goto_next_word(cmd);
	if (cmd.empty())
		this->sendToSocket(client.getSocket(), ERR_NEEDMOREPARAMS(std::string("PASS")));
	else if (client.isAuthentificated())
		this->sendToSocket(client.getSocket(), ERR_ALREADYREGISTERED);
	else if (this->password != cmd)
		this->sendToSocket(client.getSocket(), ERR_PASSWDMISMATCH);
	else
		client.authentificate();
}
