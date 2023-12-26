#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"
#include <stdlib.h>

class Server
{
    public:
        Server(std::string const port, std::string const password);
        ~Server();
        int start();
    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
};

#endif
