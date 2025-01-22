#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"

class Channel
{
  private:
	// unsigned int            max_users;
	std::string 			name;
	std::string 			password;
	std::string 			topic;
	std::vector<Client *> 	users;
	std::vector<Client *> 	users_invited;
	std::vector<Client *> 	operators;
	bool					password_required;
	bool					invitation_mode;	
	unsigned int			user_limit;

  public:
	// Constructors:
	Channel(std::string name) : name(name), invitation_mode(false), password_required(false) {};

	// Getter and setters:
	std::string 			getName() const;
	std::string 			getPassword() const;
	bool 					isInvitationMode() const;
	bool 					isPasswordRequired() const;
	void 					setPasswordRequired(bool state);
	void 					setInvitationMode(bool state);
	void 					setPassword(std::string new_password);
	std::vector<Client *>	&getUsers();
	std::vector<Client *>	&getOperators();

  public: // Members functions
	void addUser(Client *client);
	std::string getUsersString();
	Client *findUser(Client *client);
	Client *findUser(std::string nickname);
	Client *findUserInInvitationList(Client *client);
	void sendToChannel(std::string message);
	void sendToChannel(std::string message, Client *sender);
};
