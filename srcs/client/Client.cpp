#include "Client.hpp"

Client::Client(int fd) : client_socket(fd), authentificated(false)
{
	std::cerr << "[Client fd constructor]\n";
}

Client::Client() : client_socket(-1), authentificated(false)
{
	std::cerr << "[Client default constructor]\n";
}

Client::~Client()
{
	std::cerr << "[Client destructor]\n";
}

std::string Client::getNickname() const
{
	return (nickname);
}

bool Client::isAuthentificated() const
{
	return authentificated;
}
void Client::authentificate()
{
	std::cout << "Client " << client_socket << GRE << " successfully authentificated." << WHI << std::endl;
	authentificated = true;
}
