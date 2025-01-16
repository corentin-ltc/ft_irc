#include "Server.hpp"

void Server::ping(Client *client)
{
	std::string pong;
	pong = "PONG :" + client->getMessage().substr(5);
	send(client->getSocket(), pong.c_str(), pong.length(), 0);
}

void Server::handleCommand(Client *client)
{
	std::cout << "Received a command\n";
	if (client->getMessage().find("JOIN #salut\n") == 0)
	{}
	if (client->getMessage().find("PING qwdlqwd\n") == 0)
		ping(client);
}

void Server::joinChannel(Client client, std::string input)
{
	std::string channel = input.substr(5, input.size());
	if (channel.size() == 0)
		std::cout << client.getSocket() << "Usage: /join <channel_name>" << std::endl;
	std::cout << channel << std::endl;
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if (it[0].getName() == channel)
		{
			std::cout << "Client " << client.getSocket() << " has join the existing channel : " << channel << std::endl;
			it[0].addUser(client);
			break;
		}
	}
    if (it == channels.end())
	{
		Channel newChan(channel);
		channels.push_back(newChan);
		std::cout << "Client " << client.getSocket() << " successfully created the channel : " << channel << std::endl;
		newChan.addUser(client);

	}
}