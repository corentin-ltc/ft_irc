#include "Server.hpp"
#include "ft_irc.hpp"
#include <cstdlib>

void stopServer(int signal)
{
	Server::signal = true;
	(void)signal;
}

unsigned short parse_port(std::string string)
{
	unsigned long ul_port = strtoul(string.c_str(), NULL, 0);
	if (1024 > ul_port || ul_port > 49151)
		throw std::runtime_error("Specified port is out of range (1024-49151)");
	return (static_cast<unsigned short>(ul_port));
}

std::string parse_password(std::string string)
{

	return string;
}
