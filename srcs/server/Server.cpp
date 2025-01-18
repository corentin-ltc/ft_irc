#include "Server.hpp"

bool Server::signal = false;

Server::Server() : port(0), password(""), server_socket(-1)
{
	std::cout << "[Server default constructor called]" << std::endl;
}

Server::Server(unsigned short port, std::string password) : port(port), password(password), server_socket(-1)
{
	std::cout << "[Server port/password constructor called]" << std::endl;
}

Server::~Server()
{
	std::cout << "[Server destructor called]" << std::endl;
	// TODO: close every fds
	this->disconnectAll();
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
	std::cout << WHI << "Server listening on port " << port << " (server_socket: " << server_socket << ")" << std::endl;

	// NOTE: loop and wait for pollfd EVENTS
	while (this->signal == false)
	{
		if (-1 == poll(&fds[0], this->fds.size(), -1))
		{
			if (this->signal)
				break;
			throw std::runtime_error("poll");
		}
		for (size_t i = 0; i < fds.size(); i++) //-> check all file descriptors
		{
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

void Server::printInfos() const
{
	std::cout << WHI << "----------------" << YEL << " SERVER INFO" << WHI << " ----------------" << std::endl;
	std::cout << YEL << "Port : " << WHI << port << std::endl;
	std::cout << YEL << "Password : " << WHI << password << std::endl;
	std::cout << WHI << "----------------" << YEL << " CLIENTS INFO" << WHI << " ----------------" << std::endl;
	std::cout << YEL << "Clients connected : " << WHI << clients.size() << std::endl;
	for (size_t i = 0; i < clients.size(); i++)
		std::cout << clients[i].getClient() << ", socket : " << clients[i].getSocket()
				  << ", registered : " << (clients[i].isRegistered() ? "Yes" : "No") << std::endl;
	std::cout << WHI << "----------------" << YEL << " CHANNELS INFO" << WHI << " ----------------" << std::endl;
	std::cout << YEL << "Channels count : " << WHI << channels.size() << std::endl;
	for (size_t i = 0; i < channels.size(); i++)
		std::cout << channels[i].getName() << ", clients connected : " << channels[i].getUsersInChannel() << std::endl;
}
