#pragma once

#include "ft_irc.hpp"

class Channel;

class Client
{
  private:
	std::vector<Channel *> channels;
	std::string nickname;
	std::string username;
	std::string message;
	int client_socket;
	bool registered;
	bool command_ready;
	bool globalOperator;

  public: // construct
	Client(int fd);

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
	bool isGlobalOperator() const;
	// setters
	void setNickname(std::string);
	void setUsername(std::string);
	void setMessage(std::string new_message);
	void setGlobalOperator();
	void stripMessage();
	bool isMessageDone();
	void clearMessage();
	void _register();
	void setCommandReady();
	void addChannel(Channel *channel);
	// methods
	std::string getClientString() const;
	std::string getChannelsString() const;
	void leaveAllChannels();
};
