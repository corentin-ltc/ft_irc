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

std::string Client::getMessage() const
{
	return (message);
}

int Client::getSocket() const
{
	return (client_socket);
}

void Client::setMessage(std::string new_message)
{
	if (new_message.size() > 2 && new_message.find('\r') == new_message.size() - 2)
		new_message.erase(new_message.size() - 2);
	if (message.empty() || message[message.size() - 1] != '\n') // if no \n, message not done
		this->message.append(new_message);
	else
		this->message = new_message;
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
