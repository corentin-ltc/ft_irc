#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include <netinet/in.h> // struct sockaddr_in
#include <string>
#include <sys/poll.h> // struct pollfd
#include <vector>

class Server
{
  public:
	static bool signal;

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
	void acceptNewClient();
	void handleClient(int client_socket);
	void disconnectClient(int client_socket);
	void disconnectAll();
	void handleClientInput(int client_socket, std::string input);
};

#endif
