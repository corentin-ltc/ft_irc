#include "ft_irc.hpp"

int createServerSocket(int port)
{
    int serverFd;

    serverFd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: Créer un socket IPV4, SOCK_STREAM : Provides sequenced, reliable,  two-way,  connection-based  byte  streams.
    if (serverFd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1) // rend le socket non bloquant
    {
        perror("fcntl");
        close(serverFd);
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // Pouvoir se reconnecter à un port sans attendre et éviter un time out de 2 minutes du port si l'on éteint le serveur
    {
        perror("setsockopt");
        close(serverFd);
        exit(EXIT_FAILURE);
    }
}