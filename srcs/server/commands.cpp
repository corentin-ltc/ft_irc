#include "Server.hpp"

void Server::ping(int client_socket)
{
	std::cout << "got pinged" << std::endl;
	write(client_socket, "pong\n", 5);
}

void Server::handleCommand(Client *client)
{

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