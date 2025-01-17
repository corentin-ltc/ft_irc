#include "Client.hpp"
#include "Server.hpp"

Client::Client(int fd) : client_socket(fd), registered(false), command_ready(false), globalOperator(false)
{
	std::cerr << "[Client fd constructor]\n";
}

Client::Client() : client_socket(-1), registered(false)
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

std::string Client::getUsername() const
{
	return (username);
}

std::string Client::getMessage() const
{
	return (message);
}

int Client::getSocket() const
{
	return (client_socket);
}

void Client::setNickname(std::string nick)
{
	this->nickname = nick;
}

void Client::setUsername(std::string nick)
{
	this->username = nick;
}

void Client::setMessage(std::string new_message)
{
	this->message.append(new_message);
}

void Client::clearMessage()
{
	this->message.clear();
}

void Client::stripMessage()
{
	if (this->message.size() && this->message[this->message.size() - 1] == '\n')
		this->message.erase(this->message.size() - 1);
	if (this->message.size() && this->message[this->message.size() - 1] == '\r')
		this->message.erase(this->message.size() - 1);
}

bool Client::isMessageDone()
{
	if (this->message.empty() || this->message[this->message.size() - 1] != '\n') // if no \n, message not done
		return (false);
	if (this->message == "\n")
	{
		this->message.clear();
		return (false);
	}
	return (true);
}

bool Client::isRegistered() const
{
	return registered;
}
void Client::_register()
{
	registered = true;
	std::cout << "Client " << client_socket << GRE << " successfully registered." << WHI << std::endl;
}

void Client::setCommandReady()
{
	if (this->command_ready)
		return;
	if (registered && nickname.size() && username.size())
	{
		this->command_ready = true;
		Server::sendToSocket(this->client_socket, RPL_WELCOME(nickname, nickname));
	}
}

bool Client::isCommandReady() const
{
	return (this->command_ready);
}

std::vector<Channel *> &Client::getChannels()
{
	return (this->channels);
}

std::string Client::getClient() const
{
	return (nickname + "!" + username + "@localhost");
}
