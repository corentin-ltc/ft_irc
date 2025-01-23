#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ft_irc.hpp"

class Channel;

class Server
{
  public: // public static so it can be changed inside a signal handler
	static bool signal;

  private:
	unsigned short port;
	std::string password;
	int server_socket;
	struct sockaddr_in server_addr;
	std::vector<Client *> clients;
	std::vector<struct pollfd> fds;
	std::vector<Channel *> channels;

  public: // constructors
	Server();
	Server(unsigned short port, std::string password);
	~Server();

  public: // init
	void initServer();
	void run();

  public: // utils
	Client *findClient(int client_socket);
	Client *findClient(std::string nickname);
	Channel *findChannel(std::string name);
	void printInfos() const;

  private: // client communication
	void acceptNewClient();
	void handleClient(int client_socket);
	void disconnectClientFromAllChannels(Client *client, std::string reason);
	void disconnectClientFromChannel(Client *client, Channel *channel);
	void disconnectClient(Client *client);
	void disconnectAll();
	bool readClient(Client *client);

  public: // public static to be used anywhere
	static void sendToSocket(int client_socket, std::string message);

  private: // commands
	void handleCommand(Client *client, std::string cmd);
	void handleOperatorCommand(Client *client, std::string cmd, std::string cmd_name);
	void error(Client *client, std::string reason);
	void ping(int client_socket, std::string cmd);
	void pass(Client *client, std::string cmd);
	void nick(Client *client, std::string cmd);
	void user(Client *client, std::string cmd);
	void oper(Client *client, std::string cmd);
	void join(Client *client, std::string cmd);
	void privmsg(Client *client, std::string cmd);
	void part(Client *client, std::string cmd);
	void kick(Client *client, std::string cmd);
	void invite(Client *client, std::string cmd);
	void topic(Client *client, std::string cmd);
	void mode(Client *client, std::string cmd);
};

#endif
