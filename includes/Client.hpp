#pragma once

#include "ft_irc.hpp"

class Channel;

class Client
{
  private:
	std::string nickname;
	std::string username;
	std::string message;
	int client_socket;
	bool registered;
	bool command_ready;
	bool globalOperator;
	std::vector<Channel *> channels;

  public: // construct
	Client(int fd);

	bool isGlobalOperator()
	{
		return globalOperator;
	};

	Client();
	~Client();

  public:
	// getters
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getMessage() const;
	std::vector<Channel *> &getChannels();
	int getSocket() const;
	bool isRegistered() const;
	bool isCommandReady() const;
	// setters
	void setNickname(std::string);
	void setUsername(std::string);
	void setMessage(std::string new_message);
	void stripMessage();
	bool isMessageDone();
	void clearMessage();
	void _register();
	void setCommandReady();
	// methods
};
