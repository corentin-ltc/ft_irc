#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include <fcntl.h>
#include <poll.h>
#include <vector>

class Server
{
  private:
	unsigned short port;
	std::string password;
	int server_socket;
	struct sockaddr_in server_addr;
	std::vector<Client> clients;
	std::vector<struct pollfd> fds;

  public: // constructors
	Server();
	Server(unsigned short port, std::string password);
	~Server();

  public: // methods
	void initServer();
	void run();

  private: // helpers
};

#endif
