#include "Channel.hpp"
#include "ft_irc.hpp"


void    Channel::addUser(Client client)
{
    users.push_back(client);
    if (client.isGlobalOperator())
        operators.push_back(client);
    write(client.getSocketFd(), "Welcome bro\n", 13);
    
}
