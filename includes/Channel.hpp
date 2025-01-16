#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"

class Channel
{
  private:
	// unsigned int            max_users;
	std::string name;
	std::string password;
	std::vector<Client *> users;
	std::vector<Client *> operators;

  public:
	// Constructors:
	Channel(std::string name) : name(name) {};

	// Getter and setters:
	std::string getName()
	{
		return name;
	}
	// Members functions
	void addUser(Client client);
};
