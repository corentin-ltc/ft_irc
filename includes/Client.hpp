#pragma once

#include <string>

class Client
{
  private:
	std::string nickname;
	std::string username;
	int socketFd;
	bool authentificated;

  public:
	Client(int fd);
	std::string getNickname();
	bool isAuthentificated();
	void authentificate();
};
