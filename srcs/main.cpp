#include "ft_irc.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> " << "<password>"<<std::endl;
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    
    if (port <= 0) {
        std::cerr << "Port invalide." << std::endl;
        return EXIT_FAILURE;
    }



    return (0);
}