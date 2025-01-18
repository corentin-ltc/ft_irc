#include "Server.hpp"
#include "ft_irc.hpp"

void Server::acceptNewClient()
{
	int client_socket;

	client_socket = accept(server_socket, NULL, NULL);
	if (-1 == client_socket)
		throw std::runtime_error("accept");
	clients.push_back(new Client(client_socket));
	fds.push_back((struct pollfd){.fd = client_socket, .events = POLLIN, .revents = 0});
	std::cout << "Client " << client_socket << GRE << " connected." << WHI << std::endl;
}

void Server::handleClient(int client_socket)
{
	// maybe put this inside a findClient(int fd) function
	Client *client ;
	for (size_t i = 0; i < this->clients.size(); i++)
		if (this->clients[i]->getSocket() == client_socket)
			client = this->clients[i];

	this->readClient(client);
	if (client->isMessageDone() == false)
		return;
	std::cout << RED << "RECEIVED (" << client->getSocket() << "): " << WHI << client->getMessage();//DEBUG
	std::vector<std::string> cmds = split(client->getMessage(), '\n');
	for (size_t i = 0; i < cmds.size(); i++)
	{
		if (cmds[i][cmds[i].size() - 1] == '\r')
			cmds[i].erase(cmds[i].size() - 1);
		handleCommand(client, cmds[i]);
	}
	client->clearMessage();
}

void Server::readClient(Client *client)
{
	char buffer[1024];
	int bytes_read = read(client->getSocket(), &buffer, sizeof(buffer));
	if (-1 == bytes_read)
		throw std::runtime_error("read");
	if (0 == bytes_read)
	{
		disconnectClient(client->getSocket());
		return;
	}
	buffer[bytes_read] = 0;
	client->setMessage(buffer);
}

void Server::sendToSocket(int client_socket, std::string message)
{
	message.append(ENDL);
	std::cout << GRE << "SENT (" << client_socket << "): " << WHI << message;//DEBUG
	send(client_socket, message.c_str(), message.length(), SEND_FLAGS);
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
