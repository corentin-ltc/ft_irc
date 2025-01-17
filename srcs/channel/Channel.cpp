#include "Channel.hpp"
#include "ft_irc.hpp"


void    Channel::addUser(Client &client)
{
    users.push_back(&client);
    if (client.isGlobalOperator())
        operators.push_back(&client);
    std::string message = "JOIN ";
    message.append(this->name);
    message.append(client.getNickname());
	send(client.getSocket(), message.c_str(), message.length(), 0);
    std::cout << GRE << "SENT: " << WHI << message << std::endl;
    write(client.getSocket(), "Welcome bro\n", 13);
    
}
