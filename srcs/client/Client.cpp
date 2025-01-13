#include "Client.hpp"

Client::Client(int fd) : socketFd(fd), authentificated(false)
{

}

std::string Client::getNickname()
{
    return nickname;
}

bool Client::isAuthentificated()
{
    return authentificated;
}
void Client::authentificate()
{
    authentificated = true;
}