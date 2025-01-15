#include "Server.hpp"

void Server::ping(int client_socket)
{
	std::cout << "got pinged" << std::endl;
	write(client_socket, "pong\n", 5);
}

void Server::handleCommand(Client *client)
{

}