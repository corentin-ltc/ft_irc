#pragma once

#include <iostream>
#include <netinet/in.h> // serverAddr est une instance de la structure sockaddr_in, qui est utilisée pour configurer les informations réseau. Cette structure est définie dans la bibliothèque système (<netinet/in.h>).
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
