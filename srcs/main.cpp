#include "ft_irc.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
    Server serv;

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> " << "<password>"<<std::endl;
        return EXIT_FAILURE;
    }

    serv.port = atoi(argv[1]);
    
    if (serv.port <= 0) {
        std::cerr << "Invalid port." << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        serv.createServerSocket();
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    


    return (0);
}