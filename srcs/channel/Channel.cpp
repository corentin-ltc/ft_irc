#include "Channel.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"

void Channel::addUser(Client *client)
{
	users.push_back(client);
	if (client->isGlobalOperator())
		operators.push_back(client);

	std::string beginning_of_message = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost ";

	// First message
	std::string message =
		beginning_of_message + "353 " + client->getNickname() + " = " + name + " :" + getUsersInChannel();
	Server::sendToSocket(client->getSocket(), message);

	// Second message
	message = beginning_of_message + "366 " + client->getNickname() + " " + name + " :End of /NAMES list";
	Server::sendToSocket(client->getSocket(), message);

	// Third message
	message = beginning_of_message + "JOIN :" + name;
	Server::sendToSocket(client->getSocket(), message);
}

std::string Channel::getUsersInChannel() const
{
	std::string list = "";

	for (size_t i = 0; i < this->users.size(); i++)
	{
		list.append(users[i]->getNickname());
		if (i + 1 < this->users.size())
			list.append(" ");
	}
	return list;
}

void Channel::disconnectUser(Client *client)
{
	// possible BUG: erasing mid-loop might cause issues
	// NOTE: removes from users list
	for (size_t i = 0; i < users.size(); i++)
		if (client == users[i])
			users.erase(users.begin() + i);
	// NOTE: removes from operators list
	for (size_t i = 0; i < operators.size(); i++)
		if (client == operators[i])
			operators.erase(operators.begin() + i);
	// NOTE: removes channel from client
	for (size_t i = 0; i < client->getChannels().size(); i++)
		if (client->getChannels()[i] == this)
			client->getChannels().erase(client->getChannels().begin() + i);
}
