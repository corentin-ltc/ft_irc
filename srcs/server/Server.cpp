#include "Server.hpp"

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
}

void Server::run()
{
	if (-1 == server_socket)
		throw std::runtime_error("Server is not initialized");
	// TODO: make a pollfd
	// NOTE: socket becomes passive (listen)
	if (-1 == listen(server_socket, 5))
		throw std::runtime_error("listen");
	std::cout << "Server listening on port " << port << " (server_socket: " << server_socket << ")" << std::endl;

	// TODO: loop and wait for pollfd EVENTS
	// NOTE: if serv -> new client, if not -> client message
	while (1)
	{
		int client_socket = accept(server_socket, NULL, NULL);
		if (client_socket != -1)
			std::cout << "Accepted client " << client_socket << std::endl;
	}
}
