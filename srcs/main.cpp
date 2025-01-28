#include "Server.hpp"
#include <cstdlib>

// TODO: remove default arguments
int main(int argc, char **argv)
{
	unsigned short port = 6667;
	std::string password = "password";

	try
	{
		if (argc < 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		port = parse_port(argv[1]);
		password = parse_password(argv[2]);

		Server server(port, password);
		server.initServer();
		signal(SIGINT, stopServer);
		server.run();
	}
	catch (std::exception &e)
	{
		std::cout << "Couldn't run the server because : " << e.what() << std::endl;
		return (1);
	}

	return (0);
}
