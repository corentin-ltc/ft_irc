#include "Server.hpp"
#include <cstdlib>

/* TODO:
 * answer to CAP LS (might improve responsiveness)
 * delete channel if empty after a user leaves
 * channel modes (dont forget to check and print JOIN)
 */
int main(int argc, char **argv)
{
	unsigned short port = 6667;
	std::string password = "password";

	try
	{
		if (argc > 1)
			port = parse_port(argv[1]);
		if (argc > 2)
			password = parse_password(argv[2]);

		std::cout << "Port: " << port << "\nPassword: " << password << std::endl;

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
