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
	Client *client = findClient(client_socket);
	if (!client)
		return;
	// TODO: stop if the client was disconnected inside readClient (maybe check retval)
	if (this->readClient(client) == false)
		return;
	if (client->isMessageDone() == false)
		return;
	std::cout << RED << "RECEIVED (" << client->getSocket() << "): " << WHI << client->getMessage(); // DEBUG
	std::vector<std::string> cmds = split(client->getMessage(), '\n');
	for (size_t i = 0; i < cmds.size(); i++)
	{
		if (cmds[i][cmds[i].size() - 1] == '\r')
			cmds[i].erase(cmds[i].size() - 1);
		handleCommand(client, cmds[i]);
	}
	client->clearMessage();
}

bool Server::readClient(Client *client)
{
	char buffer[1024];
	int bytes_read = read(client->getSocket(), &buffer, sizeof(buffer));
	if (-1 == bytes_read)
		throw std::runtime_error("read");
	if (0 == bytes_read)
	{
		disconnectClient(client);
		return (false);
	}
	buffer[bytes_read] = 0;
	client->setMessage(buffer);
	return (true);
}

void Server::sendToSocket(int client_socket, std::string message)
{
	message.append(ENDL);
	std::cout << GRE << "SENT (" << client_socket << "): " << WHI << message; // DEBUG
	send(client_socket, message.c_str(), message.length(), SEND_FLAGS);
}

void Server::disconnectClient(Client *client)
{
	std::cout << "Client " << client->getSocket() << RED << " disconnected." << WHI << std::endl;
	// NOTE: enlever des pollfd
	for (size_t i = 0; i < this->fds.size(); i++)
	{
		if (this->fds[i].fd == client->getSocket())
		{
			close(this->fds[i].fd);
			this->fds.erase(this->fds.begin() + i);
			break;
		}
	}
	// NOTE: deco de chaque chan
	client->leaveAllChannels();
	// NOTE: enlever des users
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i] == client)
		{
			delete this->clients[i];
			this->clients[i] = NULL;
			this->clients.erase(this->clients.begin() + i);
			break;
		}
	}
}

void Server::disconnectAll()
{
	std::cout << RED << "Shutting the server down" << WHI << std::endl;
	for (size_t i = 0; i < fds.size(); i++)
		close(fds[i].fd);
	for (size_t i = 0; i < clients.size(); i++)
		delete this->clients[i];
	for (size_t i = 0; i < channels.size(); i++)
		delete channels[i];
}
