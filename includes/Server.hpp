#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "ft_irc.hpp"

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

  public: // init
	void initServer();
	void run();

  private: // client communication
	void acceptNewClient();
	void handleClient(int client_socket);
	void disconnectClient(int client_socket);
	void disconnectAll();
	void readClient(Client &client);
	static void sendToSocket(int client_socket, std::string message);

  private: // commands
	void handleCommand(Client &client, std::string cmd);
	void ping(int client_socket, std::string cmd);
	void pass(Client &client, std::string cmd);
};

#endif
