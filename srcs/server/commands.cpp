#include "Server.hpp"

inline static bool find(std::string str, std::string needle)
{
	return (str.find(needle) != std::string::npos);
}

void Server::ping(Client *client, std::string cmd)
{
	(void)cmd;
	std::string pong;
	pong = "PONG :" + client->getMessage().substr(5);
	sendToSocket(client->getSocket(), pong);
	send(client->getSocket(), pong.c_str(), pong.length(), 0);
}

void Server::handleCommand(Client *client, std::string cmd)
{
	std::cout << "Handling command: " << cmd << std::endl;
	// NOTE: cmds without auth
	if (find(cmd, "CAP"))
		return; // ignore CAP
	if (find(cmd, "PASS"))
		return;
	if (find(cmd, "USER"))
		return;
	if (find(cmd, "NICK"))
		return;
	if (find(cmd, "PING"))
		return (ping(client, cmd));
	if (client->isAuthentificated() == false)
		return; // Renvoyer une erreur client not registered/unknown command
	// NOTE: cmds needing authentification
	if (cmd.find("JOIN"))
		;
	if (cmd.find("LEAVE"))
		;
}
