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

std::vector<std::string> split(std::string source, char delimiter)
{
	std::vector<std::string> strings;
	std::string string;
	size_t pos = source.find(delimiter);
	while (pos != std::string::npos)
	{
		string = source.substr(0, pos);
		if (string.empty() == false)
			strings.push_back(string);
		source.erase(0, pos + 1);
		pos = source.find(delimiter);
	}
	return strings;
}
