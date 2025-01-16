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

void Server::ping(int client_socket, std::string cmd)
{
	goto_next_word(cmd);
	this->sendToSocket(client_socket, PONG(cmd));
}

void Server::handleCommand(Client *client, std::string cmd)
{
	std::cout << "Handling command: " << cmd << std::endl;
	// NOTE: cmds without auth
	if (find(cmd, "CAP"))
		return; // ignores CAP
	if (find(cmd, "PASS"))
		return;
	if (find(cmd, "USER"))
		return;
	if (find(cmd, "NICK"))
		return;
	if (find(cmd, "PING"))
		return (ping(client->getSocket(), cmd));
	if (client->isAuthentificated() == false)
		return; // Renvoyer une erreur client not registered/unknown command
	// NOTE: cmds needing authentification
	if (cmd.find("JOIN"))
		;
	if (cmd.find("LEAVE"))
		;
}
