#pragma once

#include "ft_irc.hpp"

class Client
{
  private:
	std::string nickname;
	std::string username;
	std::string message;
	int client_socket;
	bool authentificated;
	bool globalOperator;

  public: // construct
	Client(int fd);

	bool isGlobalOperator(){return globalOperator;};

	Client();
	~Client();

  public:
	// getters
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getMessage() const;
	int getSocket() const;
	bool isAuthentificated() const;
	// setters
	void setNickname();
	void setUsername();
	void setMessage(std::string new_message);
	void authentificate();

};
