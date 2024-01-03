#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Irc.hpp"
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
class Client
{
    public:
        Client(std::string const port, std::string const password, std::string const name);
        ~Client();
    private:
        Client(const Client &other);
        Client	&operator=(const Client &other);
        std::string _port;
        std::string _pass;
        std::string _name;
};

#endif