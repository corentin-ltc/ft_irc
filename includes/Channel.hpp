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
	std::vector<std::string> invites;
	
	bool					password_required;
	bool					invitation_mode;
	bool					can_change_topic;
	unsigned int			max_users;

  public:
	// Constructors:
	Channel(std::string name)
		: name(name) ,  password_required(false), invitation_mode(false), can_change_topic(true), max_users(__INT_MAX__){};

	// Getter and setters:
	std::string				getName() const;
	std::string				getPassword() const;
	std::vector<Client *> 	&getUsers();
	std::vector<Client *> 	&getOperators();
	unsigned int			getMaxUsers() const;
	bool 					findInvite(std::string name);
	bool 					CanEveryoneChangeTopic() const;
	void					SetEveryoneChangeTopic(bool state);
	bool 					isInvitationMode() const;
	void 					setInvitationMode(bool state);
	void 					setUserLimit(unsigned int limit);
	void 					setPasswordRequired(bool state);
	bool 					isPasswordRequired() const;
	void 					setPassword(std::string new_password);

	void addInvite(std::string name);

  public: // Members functions
	void addUser(Client *client);
	std::string getUsersString();
	Client *findUser(Client *client);
	Client *findUser(std::string nickname);
	bool isOperator(const Client *client) const;
	void setTopic(Client *client, std::string t);
	void sendToChannel(std::string message);
	void sendToChannel(std::string message, Client *sender);
};
