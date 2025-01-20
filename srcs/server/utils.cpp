#include "Server.hpp"

Client *Server::findClient(int client_socket)
{
	Client *client = NULL;
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i].getSocket() == client_socket)
			client = &this->clients[i];
	return (client);
}

Client *Server::findClient(std::string nickname)
{
	Client *client = NULL;

	if (nickname.empty())
		return (NULL);
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i].getNickname() == nickname)
			client = &this->clients[i];
	return (client);
}

Channel *Server::findChannel(std::string name)
{
	Channel *channel = NULL;

	if (name.empty())
		return (NULL);
	for (size_t i = 0; i < this->channels.size(); i++)
		if (this->channels[i]->getName() == name)
			channel = this->channels[i];
	return (channel);
}
