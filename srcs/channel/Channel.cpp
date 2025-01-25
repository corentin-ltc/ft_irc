#include "Channel.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"

std::string Channel::getName() const
{
	return (this->name);
}

bool Channel::isInvitationMode() const
{
	return (invitation_mode);
}

bool Channel::isPasswordRequired() const
{
	return (password_required);
}

void Channel::setPasswordRequired(bool state)
{
	password_required = state;
}

void Channel::setPassword(std::string new_password)
{
	password = new_password;
}

void Channel::setUserLimit(unsigned int limit)
{
	user_limit = limit;
}

void Channel::setInvitationMode(bool state)
{
	invitation_mode = state;
}

std::string Channel::getPassword() const
{
	return (this->password);
}
std::vector<Client *> &Channel::getUsers()
{
	return (this->users);
}
std::vector<Client *> &Channel::getOperators()
{
	return (this->operators);
}

void Channel::addUser(Client *client)
{
	users.push_back(client);

	// First message
	this->sendToChannel(RPL_JOIN(client->getClientString(), this->name));

	// Second message
	if (this->topic.empty() == false)
		Server::sendToSocket(client->getSocket(), RPL_TOPIC(client->getClientString(), this->name, this->topic));

	// Third message
	Server::sendToSocket(client->getSocket(), RPL_NAMREPLY(client->getClientString(), this->name, getUsersString()));
	Server::sendToSocket(client->getSocket(), RPL_ENDOFNAMES(client->getClientString(), this->name));
}

void Channel::setTopic(Client *client, std::string t)
{
	if (t == "")
		Server::sendToSocket(client->getSocket(), RPL_NOTOPIC(client->getClientString(), this->name));
	t.erase(0, 1);
	this->topic = t;
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

Client *Channel::findUser(Client *client)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i] == client)
			return (users[i]);
	}
	return (NULL);
}

Client *Channel::findUserInInvitationList(Client *client)
{
	for (size_t i = 0; i < users_invited.size(); i++)
	{
		if (users_invited[i] == client)
			return (users_invited[i]);
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

bool Channel::isOperator(const Client *client) const
{
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i] == client)
			return (true);
	}
	return (false);
}

void Channel::sendToChannel(std::string message)
{
	for (size_t i = 0; i < users.size(); i++)
		Server::sendToSocket(users[i]->getSocket(), message);
}

void Channel::sendToChannel(std::string message, Client *sender)
{
	for (size_t i = 0; i < users.size(); i++)
		if (users[i] != sender)
			Server::sendToSocket(users[i]->getSocket(), message);
}

bool Channel::findInvite(std::string name)
{
	for (size_t i = 0; i < invites.size(); i++)
		if (invites[i] == name)
			return (true);
	return (false);
}

void Channel::addInvite(std::string name)
{
	this->invites.push_back(name);
}
