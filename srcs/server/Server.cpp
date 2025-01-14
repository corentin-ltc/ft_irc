#include "Server.hpp"
#include <fcntl.h>		// fcntl()
#include <iostream>		// cout, endl
#include <stdexcept>	// runtime_error()
#include <sys/socket.h> // socket()
#include <unistd.h>		// read

bool Server::signal = false;

Server::Server() : port(0), password(""), server_socket(-1)
{
	std::cout << "[Server default constructor called]" << std::endl;
}

Server::Server(unsigned short port, std::string password) : port(port), password(password), server_socket(-1)
{
	std::cout << "[Server init constructor called]" << std::endl;
	this->initServer();
}

Server::~Server()
{
	std::cout << "[Server destructor called]" << std::endl;
	// TODO: close every fds
}
void Server::initServer()
{
	if (this->port == 0)
		throw std::runtime_error("No port specified");
	// NOTE: serv_socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == server_socket)
		throw std::runtime_error("socket");
	// set the socket option (SO_REUSEADDR) to reuse the address
	int value = 1;
	if (-1 == setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)))
		throw std::runtime_error("setsockopt");
	// make the socket non blocking
	if (-1 == fcntl(server_socket, F_SETFL, O_NONBLOCK))
		throw std::runtime_error("fcntl");

	// NOTE: adress struct
	server_addr.sin_port = htons(port);		  // converts host byte order > network byte order
	server_addr.sin_family = AF_INET;		  // IPv4 Internet Protocols
	server_addr.sin_addr.s_addr = INADDR_ANY; // Any local address // otherwise we would use

	// NOTE: bind both
	if (-1 == bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)))
		throw std::runtime_error("bind");
	std::cout << "Server initialized" << std::endl;

	// NOTE: make a pollfd
	struct pollfd serv_pfd;
	serv_pfd.fd = server_socket;
	serv_pfd.events = POLLIN; // Triggered when there is a data to read
	serv_pfd.revents = 0;
	fds.push_back(serv_pfd); // add the server pollfd to the vector
}

void Server::run()
{
	// NOTE: socket becomes passive (listen)
	if (-1 == listen(server_socket, 5))
		throw std::runtime_error("listen");
	std::cout << "Server listening on port " << port << " (server_socket: " << server_socket << ")" << std::endl;

	std::cout << "fds count: " << this->fds.size() << std::endl;
	// TODO: loop and wait for pollfd EVENTS
	while (this->signal == false)
	{
		if (-1 == poll(&fds[0], this->fds.size(), -1))
			throw std::runtime_error("poll");
		// loop through fds and to check if an event triggered
		for (size_t i = 0; i < fds.size(); i++) //-> check all file descriptors
		{
			// NOTE: if serv -> new client, if not -> client message
			if (fds[i].revents) //-> check if there is data to read
			{
				if (fds[i].fd == server_socket)
					acceptNewClient();
				else
					handleClient(fds[i].fd);
			}
		}
	}
}

void Server::acceptNewClient()
{
	struct pollfd client_pfd;
	int client_socket;

	// NOTE: accept client socket
	client_socket = accept(server_socket, NULL, NULL);
	if (-1 == client_socket)
		throw std::runtime_error("accept");
	std::cout << "Client " << client_socket << " connected" << std::endl;
	// TODO: make Client and add to Clients list
	// NOTE: make pollfd and add to fds list
	client_pfd.fd = client_socket;
	client_pfd.events = POLLIN;
	client_pfd.revents = 0;
	fds.push_back(client_pfd); // add the server pollfd to the vector
}

// TODO: pass client object
void Server::handleClient(int client_socket)
{
	char buffer[1024];
	std::cout << client_socket << " triggered an event" << std::endl;

	int bytes_read = read(client_socket, &buffer, sizeof(buffer));
	if (-1 == bytes_read)
		throw std::runtime_error("read");
	if (0 == bytes_read)
	{
		disconnectClient(client_socket);
		return;
	}
	buffer[bytes_read] = 0;
	std::cout << "received : " << buffer;
}

void Server::disconnectClient(int client_socket)
{
	// TODO: remove from clients vector
	// remove the fd from the fds vector
	// closes the fd
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == client_socket)
		{
			fds.erase(fds.begin() + i);
			close(client_socket);
			std::cout << "Client " << client_socket << " disconnected." << std::endl;
			break;
		}
	}
}
