#include "Server.hpp"

void Server::acceptNewClient()
{
	int client_socket;

	client_socket = accept(server_socket, NULL, NULL);
	if (-1 == client_socket)
		throw std::runtime_error("accept");
	clients.push_back(Client(client_socket));
	fds.push_back((struct pollfd){.fd = client_socket, .events = POLLIN, .revents = 0});
	std::cout << "Client " << client_socket << GRE << " connected." << WHI << std::endl;
}

void Server::handleAuthentification(Client *client)
{
	std::string nick;
	std::string welcome;
	if (client->getMessage().find("NICK") == 0)
		nick = client->getMessage().substr(5);
	if (client->getMessage().find("CAP LS") == 0)
	{
		std::cout << nick << std::endl;
		welcome = nick + ":001 Welcome" + nick + "\r\n";
		send(client->getSocket(), welcome.c_str(), welcome.length(), 0);
	}
	if (client->getMessage().find("PING") == 0)
	{
		welcome = "PONG :" + client->getMessage().substr(5);
		send(client->getSocket(), welcome.c_str(), welcome.length(), 0);
	}
	client->authentificate();
}

void Server::handleClient(int client_socket)
{
	Client *client;
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i].getSocket() == client_socket)
			client = &this->clients[i];

	char buffer[1024];
	int bytes_read = read(client_socket, &buffer, sizeof(buffer));
	if (-1 == bytes_read)
		throw std::runtime_error("read");
	if (0 == bytes_read)
	{
		disconnectClient(client_socket);
		return;
	}
	buffer[bytes_read] = 0;
	client->setMessage(buffer);
	std::string message = client->getMessage();
	if (message.empty() || message[message.size() - 1] != '\n') // if no \n, message not done
		return;
	if (message.size() > 2 && message[message.size() - 2] == '\r') // if \r, trims it
		message.erase(message.size() - 1);
	if (client->isAuthentificated() == 0)
		handleAuthentification(client);
	else
		handleCommand(client);
	std::cout << "message: " << message << std::endl;
}

void Server::disconnectClient(int client_socket)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == client_socket)
		{
			close(client_socket);
			this->fds.erase(this->fds.begin() + i);
			this->clients.erase(this->clients.begin() + i);
			std::cout << "Client " << client_socket << RED << " disconnected." << WHI << std::endl;
			break;
		}
	}
}

void Server::disconnectAll()
{
	std::cout << RED << "Shuting the server down" << WHI << std::endl;
	for (size_t i = 0; i < fds.size(); i++)
	{
		close(fds[i].fd);
		std::cout << "Client " << i << RED << " disconnected." << WHI << std::endl;
	}
	this->fds.clear();
	this->clients.clear();
}
