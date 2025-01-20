#include "Server.hpp"

Client *Server::findClient(int client_socket)
{
	Client *client = NULL;
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i]->getSocket() == client_socket)
			client = this->clients[i];
	return (client);
}

Client *Server::findClient(std::string nickname)
{
	Client *client = NULL;

	if (nickname.empty())
		return (NULL);
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i]->getNickname() == nickname)
			client = this->clients[i];
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

void Server::printInfos() const
{
	std::cout << WHI << "----------------" << YEL << " SERVER INFO" << WHI << " ----------------" << std::endl;
	std::cout << YEL << "Port : " << WHI << port << std::endl;
	std::cout << YEL << "Password : " << WHI << password << std::endl;
	std::cout << WHI << "----------------" << YEL << " CLIENTS INFO" << WHI << " ----------------" << std::endl;
	std::cout << YEL << "Clients connected : " << WHI << clients.size() << std::endl;
	for (size_t i = 0; i < clients.size(); i++)
		std::cout << clients[i]->getClientString() << ", socket : " << clients[i]->getSocket()
				  << ", registered : " << (clients[i]->isRegistered() ? "Yes" : "No")
				  << ", channels: " << clients[i]->getChannelsString() << std::endl;
	std::cout << WHI << "----------------" << YEL << " CHANNELS INFO" << WHI << " ----------------" << std::endl;
	std::cout << YEL << "Channels count : " << WHI << channels.size() << std::endl;
	for (size_t i = 0; i < channels.size(); i++)
		std::cout << channels[i]->getName() << ", clients connected : " << channels[i]->getUsersString() << std::endl;
}
