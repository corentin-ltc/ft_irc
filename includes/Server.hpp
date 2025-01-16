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

  public: // methods
	void initServer();
	void run();

  private: // helpers
	void acceptNewClient();
	void handleClient(int client_socket);
	void disconnectClient(int client_socket);
	void disconnectAll();
	void handleClientInput(Client client, std::string input);
	void joinChannel(Client client, std::string input);
	Client getClient(int client_socket);
	void readClient(Client &client);
	void sendToSocket(int client_socket, std::string message);

  private: // commands
	void ping(Client *client, std::string cmd);
	void handleCommand(Client *client, std::string cmd);
};

#endif
