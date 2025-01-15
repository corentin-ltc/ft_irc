#include "Server.hpp"
#include <iostream>
#include <signal.h>

void stopServer(int signal)
{
	Server::signal = true;
	(void)signal;
}

int main(void)
{
	Server server(6667, "password");

	signal(SIGINT, stopServer);

	try
	{
		server.initServer();
		server.run();
	}
	catch (std::exception &e)
	{
		std::cout << "Couldn't run the server because : " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
