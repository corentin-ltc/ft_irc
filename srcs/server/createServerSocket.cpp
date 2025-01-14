#include "ft_irc.hpp"
#include "Server.hpp"

int Server::createServerSocket()
{
    int serverFd;
    struct sockaddr_in add; // useful server address data
    struct pollfd newPoll; // monitoring file descriptors event

    add.sin_port = htons(this->port);
    add.sin_addr.s_addr = INADDR_ANY;
    add.sin_family = AF_INET;

    serverFd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: Créer un socket IPV4, SOCK_STREAM : Provides sequenced, reliable,  two-way,  connection-based  byte  streams.
    if (serverFd == -1)
        throw(std::runtime_error("failed to create socket"));

    if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1) // rend le socket non bloquant
		throw(std::runtime_error("failed to set O_NONBLOCK"));
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // Pouvoir se reconnecter à un port sans attendre et éviter un time out de 2 minutes du port si l'on éteint le serveur
		throw(std::runtime_error("failed to set SO_REUSEADDR"));
    if (bind(serverFd, (sockaddr*) &add, sizeof(add)) == -1)
		throw(std::runtime_error("failed to bind serverFd with address"));
    if (listen(serverFd, 128) == -1)
		throw(std::runtime_error("failed to listen serverFd"));
    
    newPoll.fd = serverFd;
    newPoll.events = POLLIN;
    newPoll.revents = 0;



}