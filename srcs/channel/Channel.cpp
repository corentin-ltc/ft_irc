#include "Channel.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"

std::string Channel::getName() const
{
	return (this->name);
}

std::string Channel::getPassword() const
{
	return (this->password);
}
std::vector<Client *> Channel::getUsers() const
{
	return (this->operators);
}
std::vector<Client *> Channel::getOperators() const
{
	return (this->operators);
}

void Channel::addUser(Client *client)
{
	users.push_back(client);
	if (client->isGlobalOperator())
		operators.push_back(client);

	std::string beginning_of_message = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost ";

	// First message
	std::string message =
		beginning_of_message + "353 " + client->getNickname() + " = " + name + " :" + getUsersString();
	Server::sendToSocket(client->getSocket(), message);

	// Second message
	message = beginning_of_message + "366 " + client->getNickname() + " " + name + " :End of /NAMES list";
	Server::sendToSocket(client->getSocket(), message);

	// Third message
	message = beginning_of_message + "JOIN :" + name;
	Server::sendToSocket(client->getSocket(), message);
}

std::string Channel::getUsersString()
{
	std::string list = "";

	for (std::vector<Client *>::iterator it = users.begin(); it != users.end(); it++)
	{
		list.append((*it)->getNickname());
		if (it + 1 != users.end())
			list.append(" ");
	}
	return list;
}

void Channel::disconnectUser(Client *client)
{
	// NOTE: enlever de la liste des users
	for (size_t i = 0; i < users.size(); i++)
	{
		if (client == users[i])
		{
			users.erase(users.begin() + i);
			break;
		}
	}
	// NOTE: enlever de la liste des operators
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (client == operators[i])
		{
			operators.erase(operators.begin() + i);
			break;
		}
	}
	// TODO: enlever le channel du client
	std::vector<Channel *> client_channels = client->getChannels();
	for (size_t i = 0; i < client_channels.size(); i++)
	{
		if (this == client_channels[i])
		{
			client_channels.erase(client_channels.begin() + i);
			break;
		}
	}
	// TODO: send le message a tlm
}

Client *Channel::findUser(Client *client)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i] == client)
			return (users[i]);
	}
	return (NULL);
}

Client *Channel::findUser(std::string nickname)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i]->getNickname() == nickname)
			return (users[i]);
	}
	return (NULL);
}
