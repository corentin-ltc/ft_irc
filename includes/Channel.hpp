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
	std::string topic;
	std::vector<Client *> users;
	std::vector<Client *> operators;

  public:
	// Constructors:
	Channel(std::string name) : name(name) {};

	// Getter and setters:
	std::string getName() const;
	std::string getPassword() const;
	std::vector<Client *> getUsers() const;
	std::vector<Client *> getOperators() const;

  public: // Members functions
	void addUser(Client *client);
	std::string getUsersString();
	Client *findUser(Client *client);
	Client *findUser(std::string nickname);
	void disconnectUser(Client *client);
	void sendToChannel(std::string message);
};
