#include <cstddef> // NULL
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h> // struct sockaddr_in
#include <stdexcept>
#include <sys/socket.h> // socket()
#include <unistd.h>		// read

int main(void)
{
	// NOTE: serv_socket
	int server_socket;

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
	struct sockaddr_in server_addr;
	unsigned short port = 6667;

	server_addr.sin_port = htons(port);		  // converts host byte order > network byte order
	server_addr.sin_family = AF_INET;		  // IPv4 Internet Protocols
	server_addr.sin_addr.s_addr = INADDR_ANY; // Any local address // otherwise we would use

	// NOTE: bind both
	if (-1 == bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)))
		throw std::runtime_error("bind");

	// TODO: make a pollfd
	// NOTE: socket becomes passive (listen)
	if (-1 == listen(server_socket, 5))
		throw std::runtime_error("listen");
	std::cout << "Server listening on port " << port << " (server_socket: " << server_socket << ")" << std::endl;

	// TODO: loop and wait for pollfd EVENTS
	// NOTE: if serv -> new client, if not -> client message
	int client_socket = accept(server_socket, NULL, NULL);
	std::cout << "Accepted client " << client_socket << std::endl;

	char *buffer[1024];
	ssize_t read_count = read(client_socket, &buffer, sizeof(buffer));
	if (-1 == read_count)
		throw std::runtime_error("read");
	buffer[read_count] = 0;
	std::cout << "Read: " << *buffer << std::endl;
	return (0);
}
