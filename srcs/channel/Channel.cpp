#include "Channel.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"


void    Channel::addUser(Client &client)
{
    users.push_back(&client);
    if (client.isGlobalOperator())
        operators.push_back(&client);

    std::cout << "hello\n";
    std::string beginning_of_message = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost ";
    
    // First message
    std::string message = beginning_of_message + "353 " + client.getNickname() + " = " + name + " :" + getUsersInChannel();
    Server::sendToSocket(client.getSocket(), message);

    //Second message
    message = beginning_of_message + "366 " + client.getNickname() + " " + name + " :End of /NAMES list";
    Server::sendToSocket(client.getSocket(), message);

    //Third message
    message = beginning_of_message + "JOIN :" + name;
    Server::sendToSocket(client.getSocket(), message);

}


std::string    Channel::getUsersInChannel()
{
    std::string list = "";
    for (std::vector<Client*>::iterator it = users.begin(); it != users.end(); it++)
    {
        list.append((*it)->getNickname());
        if (it + 1 != users.end())
            list.append(" ");
    }
    return list;
}
