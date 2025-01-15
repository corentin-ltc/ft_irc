#pragma once

#include <string>

class Client
{
  private:
	std::string nickname;
	std::string username;
	int socketFd;
	bool authentificated;
	bool globalOperator;

  public:
	Client(int fd);
	std::string getNickname(){return nickname;};
	int getSocketFd(){return socketFd;};
	bool isGlobalOperator(){return globalOperator;};
	bool isAuthentificated(){return authentificated;};
	void authentificate(){authentificated = true;};
};
