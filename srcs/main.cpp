#include "Server.hpp"

int main(void)
{
	Server server(6667, "password");

	server.run();
	return (0);
}
